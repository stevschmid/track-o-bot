#include "Uploader.h"

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

DEFINE_SINGLETON_SCOPE( Uploader );

Uploader::Uploader() {
  connect( &mNetworkManager, SIGNAL( sslErrors(QNetworkReply*, const QList<QSslError>&) ),
      this, SLOT( SSLErrors(QNetworkReply*, const QList<QSslError>&) ) );
}

Uploader::~Uploader() {
}

void Uploader::EnsureAccountIsSetUp() {
  if( !IsAccountSetUp() ) {
    LOG( "No account setup. Creating one for you." );
    CreateAndStoreAccount();
  } else {
    LOG( "Account %s found", Username().toStdString().c_str() );
  }
}

void Uploader::UploadResult( const QJsonObject& result )
{
  QJsonObject params;
  params[ "result" ] = result;

  // Some metadata to find out room for improvement
  QJsonArray meta;
  meta.append( Hearthstone::Instance()->Width() );
  meta.append( Hearthstone::Instance()->Height() );
  meta.append( VERSION );
  meta.append( PLATFORM );
  params[ "_meta" ] = meta;

  QByteArray data = QJsonDocument( params ).toJson();

  QNetworkReply *reply = AuthPostJson( "/profile/results.json", data );
  connect( reply, &QNetworkReply::finished, [&, reply, result]() {
    if( reply->error() == QNetworkReply::NoError ) {
      emit UploadResultSucceeded( result );
    } else {
      int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
      emit UploadResultFailed( result, statusCode );
    }
  });
}

QNetworkReply* Uploader::AuthPostJson( const QString& path, const QByteArray& data ) {
  QString credentials = "Basic " + ( Username() + ":" + Password() ).toLatin1().toBase64();

  QNetworkRequest request = CreateUploaderRequest( path );
  request.setRawHeader( "Authorization", credentials.toLatin1() );
  request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
  return mNetworkManager.post( request, data );
}

QNetworkRequest Uploader::CreateUploaderRequest( const QString& path ) {
  QUrl url( WebserviceURL( path ) );
  QNetworkRequest request( url );
  request.setRawHeader( "User-Agent", "Track-o-Bot/" VERSION PLATFORM );
  return request;
}

void Uploader::CreateAndStoreAccount() {
  QNetworkRequest request = CreateUploaderRequest( "/users.json" );
  QNetworkReply *reply = mNetworkManager.post( request, "" );
  connect( reply, SIGNAL(finished()), this, SLOT(CreateAndStoreAccountHandleReply()) );
}

void Uploader::CreateAndStoreAccountHandleReply() {
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

void Uploader::OpenProfile() {
  QNetworkReply *reply = AuthPostJson( "/one_time_auth.json", "" );
  connect( reply, SIGNAL( finished() ), this, SLOT( OpenProfileHandleReply() ) );
}

void Uploader::OpenProfileHandleReply() {
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

QString Uploader::Username() const {
  return mSettings.value( "username" ).toString();
}

QString Uploader::Password() const {
  return mSettings.value( "password" ).toString();
}

QString Uploader::WebserviceURL( const QString& path ) {
  return WebserviceURL() + path;
}

QString Uploader::WebserviceURL() {
  if( !mSettings.contains( "webserviceUrl" ) || mSettings.value( "webserviceUrl" ).toString().isEmpty() ) {
    SetWebserviceURL( DEFAULT_WEBSERVICE_URL );
  }

  return mSettings.value( "webserviceUrl" ).toString();
}

void Uploader::SetUsername( const QString& username ) {
  mSettings.setValue( "username", username );
}

void Uploader::SetPassword( const QString& password ) {
  mSettings.setValue( "password", password );
}

void Uploader::SetWebserviceURL( const QString& webserviceUrl ) {
  mSettings.setValue( "webserviceUrl", webserviceUrl );
}

bool Uploader::IsAccountSetUp() const {
  return mSettings.contains( "username" ) && mSettings.contains( "password" ) &&
    !mSettings.value( "username" ).toString().isEmpty() && !mSettings.value( "password" ).toString().isEmpty();
}

// Allow self-signed certificates because Qt might report
// "There root certificate of the certificate chain is self-signed, and untrusted"
// The root cert might not be trusted yet (only after we browse to the website)
// So allow allow self-signed certificates, just in case
void Uploader::SSLErrors(QNetworkReply *reply, const QList<QSslError>& errors) {
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

