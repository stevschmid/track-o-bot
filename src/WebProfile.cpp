#include "WebProfile.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QTimer>
#include <QDesktopServices>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "Hearthstone.h"

#include "Settings.h"

#define DEFAULT_WEBSERVICE_URL "https://trackobot.com"

DEFINE_SINGLETON_SCOPE( WebProfile );

WebProfile::WebProfile() {
  connect( &mNetworkManager, SIGNAL( sslErrors(QNetworkReply*, const QList<QSslError>&) ),
      this, SLOT( SSLErrors(QNetworkReply*, const QList<QSslError>&) ) );
  connect( Settings::Instance(), SIGNAL( OpenProfileRequested() ),
      this, SLOT( OpenProfile() ) );
}

WebProfile::~WebProfile() {
}

void WebProfile::EnsureAccountIsSetUp() {
  if( !Settings::Instance()->HasAccount() ) {
    LOG( "No account setup. Creating one for you." );
    CreateAndStoreAccount();
  } else {
    LOG( "Account %s found", Settings::Instance()->AccountUsername().toStdString().c_str() );
  }
}

void WebProfile::UploadResult( const QJsonObject& result )
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

QNetworkReply* WebProfile::AuthPostJson( const QString& path, const QByteArray& data ) {
  QString credentials = "Basic " +
    ( Settings::Instance()->AccountUsername() +
      ":" +
      Settings::Instance()->AccountPassword()
    ).toLatin1().toBase64();

  QNetworkRequest request = CreateWebProfileRequest( path );
  request.setRawHeader( "Authorization", credentials.toLatin1() );
  request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
  return mNetworkManager.post( request, data );
}

QNetworkRequest WebProfile::CreateWebProfileRequest( const QString& path ) {
  QUrl url( WebserviceURL( path ) );
  QNetworkRequest request( url );
  request.setRawHeader( "User-Agent", "Track-o-Bot/" VERSION PLATFORM );
  return request;
}

void WebProfile::CreateAndStoreAccount() {
  QNetworkRequest request = CreateWebProfileRequest( "/users.json" );
  QNetworkReply *reply = mNetworkManager.post( request, "" );
  connect( reply, SIGNAL(finished()), this, SLOT(CreateAndStoreAccountHandleReply()) );
}

void WebProfile::CreateAndStoreAccountHandleReply() {
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

      Settings::Instance()->SetAccount(
          user["username"].toString(),
          user["password"].toString() );
    }
  } else {
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    ERR( "There was a problem creating an account. Error: %i HTTP Status Code: %i", reply->error(), statusCode );
  }
}

void WebProfile::OpenProfile() {
  QNetworkReply *reply = AuthPostJson( "/one_time_auth.json", "" );
  connect( reply, SIGNAL( finished() ), this, SLOT( OpenProfileHandleReply() ) );
}

void WebProfile::OpenProfileHandleReply() {
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

QString WebProfile::WebserviceURL( const QString& path ) {
  if( Settings::Instance()->WebserviceURL().isEmpty() ) {
    Settings::Instance()->SetWebserviceURL( DEFAULT_WEBSERVICE_URL );
  }

  return Settings::Instance()->WebserviceURL() + path;
}

// Allow self-signed certificates because Qt might report
// "There root certificate of the certificate chain is self-signed, and untrusted"
// The root cert might not be trusted yet (only after we browse to the website)
// So allow allow self-signed certificates, just in case
void WebProfile::SSLErrors(QNetworkReply *reply, const QList<QSslError>& errors) {
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

