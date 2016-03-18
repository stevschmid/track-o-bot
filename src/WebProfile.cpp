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

#if defined(Q_OS_MAC) && !defined(QT_NO_BEARERMANAGEMENT)
  #error Qt must be built without bearer management (-no-feature-bearermanagement) on Mac (until https://bugreports.qt.io/browse/QTBUG-50181 is fixed)
#endif

WebProfile::WebProfile( QObject *parent )
  : QObject( parent )
{
  connect( &mNetworkManager, &QNetworkAccessManager::sslErrors, this, &WebProfile::SSLErrors );
}

bool JsonFromReply( QNetworkReply *reply, QJsonObject *object ) {
  QByteArray jsonData = reply->readAll();
  QJsonParseError error;
  *object = QJsonDocument::fromJson( jsonData, &error ).object();

  if( error.error != QJsonParseError::NoError ) {
    ERR( "Couldn't parse response %s", qt2cstr( error.errorString() ) );
    return false;
  }

  DBG( "Received %s", qt2cstr( QString( jsonData ) ) );
  return true;
}

void WebProfile::EnsureAccountIsSetUp() {
  if( !Settings::Instance()->HasAccount() ) {
    LOG( "No account setup. Creating one for you." );
    CreateAndStoreAccount();
  } else {
    LOG( "Account %s found", qt2cstr( Settings::Instance()->AccountUsername() ) );
  }
}

void WebProfile::UploadResult( const QJsonObject& result )
{
  QJsonObject params;
  params[ "result" ] = result;

  // Optional upload metadata to find out room for improvements
  if( Settings::Instance()->DebugEnabled() ) {
    METADATA( "GAME_WIDTH", Hearthstone::Instance()->Width() );
    METADATA( "GAME_HEIGHT", Hearthstone::Instance()->Height() );
    METADATA( "TOB_VERSION", VERSION );
    METADATA( "PLATFORM", PLATFORM );

    QJsonObject meta;
    for( auto it : Metadata::Instance()->Map().toStdMap() ) {
      const QString& key = it.first;
      const QString& value = it.second;

      meta[ key ] = value;
    }

    params[ "_meta" ] = meta;
  }
  Metadata::Instance()->Clear();

  QByteArray data = QJsonDocument( params ).toJson();

  QNetworkReply *reply = AuthPostJson( "/profile/results.json", data );
  connect( reply, &QNetworkReply::finished, [&, reply, result]() {
    int replyCode = reply->error();

    if( replyCode == QNetworkReply::NoError ) {
      QJsonObject response;
      if( JsonFromReply( reply, &response) ) {
        emit UploadResultSucceeded( response );
      }
    } else {
      int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
      emit UploadResultFailed( result, replyCode, statusCode );
    }

    reply->deleteLater();
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
  connect( reply, &QNetworkReply::finished, this, &WebProfile::CreateAndStoreAccountHandleReply );
}

void WebProfile::CreateAndStoreAccountHandleReply() {
  QNetworkReply *reply = static_cast< QNetworkReply* >( sender() );
  if( reply->error() == QNetworkReply::NoError ) {
    LOG( "Account creation was successful!" );

    QJsonObject user;
    if( JsonFromReply( reply, &user ) ) {
      LOG( "Welcome %s", qt2cstr( user[ "username" ].toString() ) );

      Settings::Instance()->SetAccount(
          user["username"].toString(),
          user["password"].toString() );
    }
  } else {
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    ERR( "There was a problem creating an account. Error: %i HTTP Status Code: %i", reply->error(), statusCode );
  }

  reply->deleteLater();
}

void WebProfile::OpenProfile() {
  QNetworkReply *reply = AuthPostJson( "/one_time_auth.json", "" );
  connect( reply, &QNetworkReply::finished, this, &WebProfile::OpenProfileHandleReply );
}

void WebProfile::OpenProfileHandleReply() {
  QNetworkReply *reply = static_cast< QNetworkReply* >( sender() );
  if( reply->error() == QNetworkReply::NoError ) {
    QJsonObject response;
    if( JsonFromReply( reply, &response ) ) {
      QString url = response[ "url" ].toString();
      QDesktopServices::openUrl( QUrl( url ) );
    }
  } else {
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    ERR( "There was a problem creating an auth token. Error: %i HTTP Status Code: %i", reply->error(), statusCode );
  }

  reply->deleteLater();
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
    } else {
      ERR( "SSL Error %d", err.error() );
    }
  }

  reply->ignoreSslErrors( errorsToIgnore );
}

