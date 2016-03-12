#include "Overlay.h"
#include "ui_Overlay.h"

#include "../Hearthstone.h"

#include <objc/objc-runtime.h>
#include <cassert>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QPainter>

Overlay::Overlay( QWidget *parent )
  : QMainWindow( parent ), mUI( new Ui::Overlay )
{
  mUI->setupUi( this );
  setWindowFlags( Qt::NoDropShadowWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Window );
  setAttribute( Qt::WA_TranslucentBackground );

  QFont newFont = font();
  newFont.setPointSize(newFont.pointSize() + 20);
  setFont(newFont);

  connect( Hearthstone::Instance(), &Hearthstone::GameWindowChanged, this, &Overlay::HandleGameWindowChanged );
  connect( Hearthstone::Instance(), &Hearthstone::GameStarted, this, &Overlay::HandleGameStarted );
  connect( Hearthstone::Instance(), &Hearthstone::GameStopped, this, &Overlay::HandleGameStopped );

#ifdef Q_OS_MAC
  WId windowObject = this->winId();
  objc_object* nsviewObject = reinterpret_cast<objc_object*>(windowObject);
  objc_object* nsWindowObject = objc_msgSend( nsviewObject, sel_registerName("window") );
  int NSWindowCollectionBehaviorCanJoinAllSpaces = 1 << 0;
  objc_msgSend( nsWindowObject, sel_registerName("setCollectionBehavior:"), NSWindowCollectionBehaviorCanJoinAllSpaces );
#endif

  LoadCards();

  hide();
}

Overlay::~Overlay() {
  delete mUI;
}

void Overlay::LoadCards() {
  QFile file( ":/data/cards.json" );
  bool opened = file.open( QIODevice::ReadOnly | QIODevice::Text );
  assert( opened );

  QByteArray jsonData = file.readAll();
  QJsonParseError error;
  QJsonArray jsonCards = QJsonDocument::fromJson( jsonData, &error ).array();
  assert( error.error == QJsonParseError::NoError );

  mCardDB.clear();
  LOG("LoadCards");

  for( QJsonValueRef jsonCardRef : jsonCards ) {
    QJsonObject jsonCard = jsonCardRef.toObject();
    LOG( "Add card %s", qt2cstr( jsonCard["id"].toString() ) );
    mCardDB[ jsonCard["id"].toString() ] = jsonCard.toVariantMap();
  }
}

void Overlay::paintEvent( QPaintEvent* ) {
  QPainter painter(this);

  QFontMetrics metrics(font());

  int playerY = 100, opponentY = 100;
  int spacing = metrics.ascent() - metrics.descent();

  QPen pen( QColor( 255, 255, 255, 150 ) );
  painter.setPen( pen );

  for( const QVariantMap& it : mOpponentHistory ) {
    painter.drawText( 0.1 * width(),
        opponentY,
        QString( "[%1] %2 (%3)" ).arg( it["mana"].toInt() ).arg( it["name"].toString() ).arg( it["count"].toInt() ) );
    opponentY += spacing;
  }

  for( const QVariantMap& it : mPlayerHistory ) {
    painter.drawText( 0.8 * width(),
        playerY,
        QString( "[%1] %2 (%3)" ).arg( it["mana"].toInt() ).arg( it["name"].toString() ).arg( it["count"].toInt() ) );
    playerY += spacing;
  }
}

void Overlay::HandleGameWindowChanged( int x, int y, int w, int h ) {
  LOG( "HandleGameWindowChanged %d %d %d %d", x, y, w, h );
  move( x, y );
  setFixedSize( w, h );
  update();
}

void Overlay::HandleGameStarted() {
  show();
}

void Overlay::HandleGameStopped() {
  hide();
}

void Overlay::HandleCardHistoryListUpdate( const ::CardHistoryList& cardHistoryList ) {
  QMap< QString, QVariantMap > player;
  QMap< QString, QVariantMap > opponent;

  for( const CardHistoryItem& it : cardHistoryList ) {
    QString cardId = QString( it.cardId.c_str() );

    if( !mCardDB.contains( cardId ) ) {
      DBG( "Card %s not found", qt2cstr( cardId ) );
      continue;
    }

    QMap< QString, QVariantMap > *ref;
    if( it.player == PLAYER_SELF ) {
      ref = &player;
    } else {
      ref = &opponent;
    }

    QVariantMap& entry = (*ref)[ cardId ];
    entry[ "count" ] = entry.value( "count", 0 ).toInt() + 1;
    entry[ "mana" ] = mCardDB[ cardId ][ "mana" ];
    entry[ "name" ] = mCardDB[ cardId ][ "name" ];
  }

  mPlayerHistory = player.values();
  qSort( mPlayerHistory.begin(), mPlayerHistory.end(), []( const QVariantMap& a, const QVariantMap& b ) {
    if( a["mana"].toInt() == b["mana"].toInt() ) {
      return a["name"].toString() < b["name"].toString();
    } else {
      return a["mana"].toInt() < b["mana"].toInt();
    }
  });

  mOpponentHistory = opponent.values();
  qSort( mOpponentHistory.begin(), mOpponentHistory.end(), []( const QVariantMap& a, const QVariantMap& b ) {
    if( a["mana"].toInt() == b["mana"].toInt() ) {
      return a["name"].toString() < b["name"].toString();
    } else {
      return a["mana"].toInt() < b["mana"].toInt();
    }
  });

  update();
}
