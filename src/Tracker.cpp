#include "Tracker.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QTimer>
#include <QDesktopServices>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "Hearthstone.h"

#define DEFAULT_WEBSERVICE_URL "https://trackobot.com"

DEFINE_SINGLETON_SCOPE( Tracker );

Tracker::Tracker() {
  qRegisterMetaType< Result >( "Result" );

  connect( &mNetworkManager, SIGNAL( sslErrors(QNetworkReply*, const QList<QSslError>&) ),
      this, SLOT( SSLErrors(QNetworkReply*, const QList<QSslError>&) ) );
}

Tracker::~Tracker() {
}

void Tracker::EnsureAccountIsSetUp() {
  if( !IsAccountSetUp() ) {
    LOG( "No account setup. Creating one for you." );
    CreateAndStoreAccount();
  } else {
    LOG( "Account %s found", Username().toStdString().c_str() );
  }
}

void Tracker::UploadResult( const Result& res )
{
  LOG( "Upload %s %s vs. %s as %s. Went %s",
      MODE_NAMES[ res.mode ],
      OUTCOME_NAMES[ res.outcome ],
      CLASS_NAMES[ res.opponent ],
      CLASS_NAMES[ res.hero ],
      ORDER_NAMES[ res.order ] );

  QJsonObject params;
  params[ "result" ] = res.AsJson();

  // Some metadata to find out room for improvement
  QJsonArray meta;
  meta.append( Hearthstone::Instance()->Width() );
  meta.append( Hearthstone::Instance()->Height() );
  meta.append( VERSION );
  meta.append( PLATFORM );
  params[ "_meta" ] = meta;

  QByteArray data = QJsonDocument( params ).toJson();

  QNetworkReply *reply = AuthPostJson( "/profile/results.json", data );
  connect( reply, &QNetworkReply::finished, [&, reply, res]() {
    if( reply->error() == QNetworkReply::NoError ) {
      LOG( "Result was uploaded successfully!" );
    } else {
      int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
      ERR( "There was a problem uploading the result. Error: %i HTTP Status Code: %i", reply->error(), statusCode );
      emit UploadResultFailed( res );
    }
  });
}

QNetworkReply* Tracker::AuthPostJson( const QString& path, const QByteArray& data ) {
  QString credentials = "Basic " + ( Username() + ":" + Password() ).toLatin1().toBase64();

  QNetworkRequest request = CreateTrackerRequest( path );
  request.setRawHeader( "Authorization", credentials.toLatin1() );
  request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
  return mNetworkManager.post( request, data );
}

QNetworkRequest Tracker::CreateTrackerRequest( const QString& path ) {
  QUrl url( WebserviceURL( path ) );
  QNetworkRequest request( url );
  request.setRawHeader( "User-Agent", "Track-o-Bot/" VERSION PLATFORM );
  return request;
}

void Tracker::CreateAndStoreAccount() {
  QNetworkRequest request = CreateTrackerRequest( "/users.json" );
  QNetworkReply *reply = mNetworkManager.post( request, "" );
  connect( reply, SIGNAL(finished()), this, SLOT(CreateAndStoreAccountHandleReply()) );
}

void Tracker::CreateAndStoreAccountHandleReply() {
  QNetworkReply *reply = static_cast< QNetworkReply* >( sender() );
  if( reply->error() == QNetworkReply::NoError ) {
    LOG( "Account creation was successful!" );

    QByteArray jsonData = reply->readAll();

    QJsonParseError error;
    QJsonObject user = QJsonDocument::fromJson( jsonData, &error ).object();

    if( error.error != QJsonParseError::NoError ) {
      ERR( "Couldn't parse response %s", error.errorString().toStdString().c_str() );
    } else {
      LOG( "Welcome %s", user[ "username" ].toString().toStdString().c_str() );

      SetUsername( user["username"].toString() );
      SetPassword( user["password"].toString() );

      emit AccountCreated();
    }
  } else {
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    ERR( "There was a problem creating an account. Error: %i HTTP Status Code: %i", reply->error(), statusCode );
  }
}

void Tracker::OpenProfile() {
  QNetworkReply *reply = AuthPostJson( "/one_time_auth.json", "" );
  connect( reply, SIGNAL( finished() ), this, SLOT( OpenProfileHandleReply() ) );
}

void Tracker::OpenProfileHandleReply() {
  QNetworkReply *reply = static_cast< QNetworkReply* >( sender() );
  if( reply->error() == QNetworkReply::NoError ) {
    QByteArray jsonData = reply->readAll();

    QJsonParseError error;
    QJsonObject response = QJsonDocument::fromJson( jsonData, &error ).object();

    if( error.error != QJsonParseError::NoError ) {
      ERR( "Couldn't parse response %s", error.errorString().toStdString().c_str() );
    } else {
      QString url = response[ "url" ].toString();
      QDesktopServices::openUrl( QUrl( url ) );
    }
  } else {
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    ERR( "There was a problem creating an auth token. Error: %i HTTP Status Code: %i", reply->error(), statusCode );
  }
}

QString Tracker::Username() const {
  return mSettings.value( "username" ).toString();
}

QString Tracker::Password() const {
  return mSettings.value( "password" ).toString();
}

QString Tracker::WebserviceURL( const QString& path ) {
  return WebserviceURL() + path;
}

QString Tracker::WebserviceURL() {
  if( !mSettings.contains( "webserviceUrl" ) || mSettings.value( "webserviceUrl" ).toString().isEmpty() ) {
    SetWebserviceURL( DEFAULT_WEBSERVICE_URL );
  }

  return mSettings.value( "webserviceUrl" ).toString();
}

void Tracker::SetUsername( const QString& username ) {
  mSettings.setValue( "username", username );
}

void Tracker::SetPassword( const QString& password ) {
  mSettings.setValue( "password", password );
}

void Tracker::SetWebserviceURL( const QString& webserviceUrl ) {
  mSettings.setValue( "webserviceUrl", webserviceUrl );
}

bool Tracker::IsAccountSetUp() const {
  return mSettings.contains( "username" ) && mSettings.contains( "password" ) &&
    !mSettings.value( "username" ).toString().isEmpty() && !mSettings.value( "password" ).toString().isEmpty();
}

// Allow self-signed certificates because Qt might report
// "There root certificate of the certificate chain is self-signed, and untrusted"
// The root cert might not be trusted yet (only after we browse to the website)
// So allow allow self-signed certificates, just in case
void Tracker::SSLErrors(QNetworkReply *reply, const QList<QSslError>& errors) {
  QList<QSslError> errorsToIgnore;

  for( const QSslError& err : errors ) {
    if( err.error() == QSslError::SelfSignedCertificate ||
        err.error() == QSslError::SelfSignedCertificateInChain )
    {
      errorsToIgnore << err;
    }
  }

  reply->ignoreSslErrors( errorsToIgnore );
}

