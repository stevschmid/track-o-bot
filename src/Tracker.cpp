#include "Tracker.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QTimer>
#include <QDesktopServices>

#include "Json.h"
#include "Hearthstone.h"

#define DEFAULT_WEBSERVICE_URL "https://trackobot.com"

DEFINE_SINGLETON_SCOPE( Tracker );

Tracker::Tracker() {
  mSuccessfulResultCount = 0;
  mUnknownOutcomeCount = 0;
  mUnknownModeCount = 0;
  mUnknownOrderCount = 0;
  mUnknownClassCount = 0;
  mUnknownOpponentCount = 0;

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

void Tracker::AddResult( GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass,
    const CardHistoryList& historyCardList, int durationInSeconds, int rank, int legend )
{
  if( mode == MODE_SOLO_ADVENTURES ) {
    LOG( "Ignore solo adventure" );
    return;
  }

  if( mode == MODE_TAVERN_BRAWL ) {
    LOG( "Ignore tavern brawl" );
    return;
  }

#ifdef _DEBUG
  string cardHistoryOutput;
  for( CardHistoryList::const_iterator it = historyCardList.begin(); it != historyCardList.end(); ++it ) {
    cardHistoryOutput += (*it).player == PLAYER_SELF ? "SELF " : "OPPONENT ";
    cardHistoryOutput += (*it).cardId + "\n";
  }
  DEBUG( "Card History: %s", cardHistoryOutput.c_str() );
#endif

  if( outcome == OUTCOME_UNKNOWN ) {
    mUnknownOutcomeCount++;
    LOG( "Outcome unknown. Skip result" );
    return;
  }

  if( mode == MODE_UNKNOWN ) {
    mUnknownModeCount++;
    LOG( "Mode unknown. Skip result" );
    return;
  }

  if( order == ORDER_UNKNOWN ) {
    mUnknownOrderCount++;
    LOG( "Order unknown. Skip result" );
    return;
  }

  if( ownClass == CLASS_UNKNOWN ) {
    mUnknownClassCount++;
    LOG( "Own Class unknown. Skip result" );
    return;
  }

  if( opponentClass == CLASS_UNKNOWN ) {
    mUnknownOpponentCount++;
    LOG( "Class of Opponent unknown. Skip result" );
    return;
  }

  mSuccessfulResultCount++;

  LOG( "Upload %s %s vs. %s as %s. Went %s",
      MODE_NAMES[ mode ],
      OUTCOME_NAMES[ outcome ],
      CLASS_NAMES[ opponentClass ],
      CLASS_NAMES[ ownClass ],
      ORDER_NAMES[ order ] );

  QtJson::JsonObject result;
  result[ "coin" ]     = ( order == ORDER_SECOND );
  result[ "hero" ]     = CLASS_NAMES[ ownClass ];
  result[ "opponent" ] = CLASS_NAMES[ opponentClass ];
  result[ "win" ]      = ( outcome == OUTCOME_VICTORY );
  result[ "mode" ]     = MODE_NAMES[ mode ];
  result[ "duration" ] = durationInSeconds;

  if( mode == MODE_RANKED && rank != RANK_UNKNOWN && legend == LEGEND_UNKNOWN ) {
    result[ "rank" ] = rank;
  }
  if( mode == MODE_RANKED && legend != LEGEND_UNKNOWN ) {
    result[ "legend" ] = legend;
  }

  QtJson::JsonArray card_history;
  for( CardHistoryList::const_iterator it = historyCardList.begin(); it != historyCardList.end(); ++it ) {
    QtJson::JsonObject item;
    item[ "turn" ] = (*it).turn;
    item[ "player" ] = (*it).player == PLAYER_SELF ? "me" : "opponent";
    item[ "card_id" ] = (*it).cardId.c_str();
    card_history.append(item);
  }
  result[ "card_history" ] = card_history;

  QtJson::JsonObject params;
  params[ "result" ] = result;

  // Some metadata to find out room for improvement
  QtJson::JsonArray meta;

  meta.append( mSuccessfulResultCount );
  meta.append( mUnknownOutcomeCount );
  meta.append( mUnknownModeCount );
  meta.append( mUnknownOrderCount );
  meta.append( mUnknownClassCount );
  meta.append( mUnknownOpponentCount );
  meta.append( Hearthstone::Instance()->Width() );
  meta.append( Hearthstone::Instance()->Height() );
  meta.append( VERSION );
  meta.append( PLATFORM );

  params[ "_meta" ] = meta;

  QByteArray data = QtJson::serialize( params );

  QNetworkReply *reply = AuthPostJson( "/profile/results.json", data );
  connect( reply, SIGNAL( finished() ), this, SLOT( AddResultHandleReply() ) );
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

void Tracker::AddResultHandleReply() {
  QNetworkReply *reply = static_cast< QNetworkReply* >( sender() );
  if( reply->error() == QNetworkReply::NoError ) {
    LOG( "Result was uploaded successfully!" );
  } else {
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    LOG( "There was a problem uploading the result. Error: %i HTTP Status Code: %i", reply->error(), statusCode );
  }
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

    bool ok;
    QtJson::JsonObject user = QtJson::parse( jsonData, ok ).toMap();

    if( !ok ) {
      LOG( "Couldn't parse response" );
    } else {
      LOG( "Welcome %s", user[ "username" ].toString().toStdString().c_str() );

      SetUsername( user["username"].toString() );
      SetPassword( user["password"].toString() );

      emit AccountCreated();
    }
  } else {
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    LOG( "There was a problem creating an account. Error: %i HTTP Status Code: %i", reply->error(), statusCode );
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

    bool ok;
    QtJson::JsonObject response = QtJson::parse( jsonData, ok ).toMap();

    if( !ok ) {
      LOG( "Couldn't parse response" );
    } else {
      QString url = response[ "url" ].toString();
      QDesktopServices::openUrl( QUrl( url ) );
    }
  } else {
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    LOG( "There was a problem creating an auth token. Error: %i HTTP Status Code: %i", reply->error(), statusCode );
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

  QList<QSslError>::const_iterator cit;
  for( cit = errors.begin(); cit != errors.end(); ++cit ) {
    const QSslError& err = *cit;
    if( err.error() == QSslError::SelfSignedCertificate ||
        err.error() == QSslError::SelfSignedCertificateInChain )
    {
      errorsToIgnore << err;
    }
  }

  reply->ignoreSslErrors( errorsToIgnore );
}

