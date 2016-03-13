#include "Overlay.h"
#include "ui_Overlay.h"

#include "../Hearthstone.h"

#include <objc/objc-runtime.h>
#include <cassert>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

Overlay::Overlay( QWidget *parent )
  : QMainWindow( parent ), mUI( new Ui::Overlay )
{
  mUI->setupUi( this );
  setWindowFlags( Qt::NoDropShadowWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Window );
  setAttribute( Qt::WA_TranslucentBackground );

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

  for( QJsonValueRef jsonCardRef : jsonCards ) {
    QJsonObject jsonCard = jsonCardRef.toObject();
    mCardDB[ jsonCard["id"].toString() ] = jsonCard.toVariantMap();
  }
}

void Overlay::paintEvent( QPaintEvent* ) {
  QPainter painter(this);

  QFont newFont = font();
  newFont.setPointSize( 20 );
  setFont( newFont );

  QPen pen( QColor( 255, 255, 255, 200 ) );
  painter.setPen( pen );
  PaintHistory( painter, 20, 0.15 * height(), mOpponentHistory );
  PaintHistory( painter, width() - 200, 0.15 * height(), mPlayerHistory );
}

void Overlay::PaintHistory( QPainter& painter, int x, int y, QList< QVariantMap >& history ) {
  QFontMetrics metrics(font());
  int spacing = (metrics.ascent() - metrics.descent()) * 1.4;

  for( const QVariantMap& it : history ) {
    QString line;
    if( it["count"].toInt() > 1 ) {
      line = QString( "%1x%2" ).arg( it["count"].toInt() ).arg( it["name"].toString() );
    } else {
      line = QString( "%1" ).arg( it["name"].toString() );
    }

    painter.drawText( x, y, line );
    y += spacing;
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

void Overlay::UpdateHistoryFor( Player player, const ::CardHistoryList& list ) {
  QMap< QString, QVariantMap > entries;

  for( const CardHistoryItem& it : list ) {
    const QString& cardId = it.cardId;

    if( !mCardDB.contains( cardId ) ) {
      DBG( "Card %s not found", qt2cstr( cardId ) );
      continue;
    }

    if( mCardDB[ cardId ][ "type" ] == "hero" ) {
      continue;
    }

    if( it.player != player ) {
      continue;
    }

    QVariantMap& entry = entries[ cardId ];
    entry[ "count" ] = entry.value( "count", 0 ).toInt() + 1;
    entry[ "mana" ] = mCardDB[ cardId ][ "mana" ];
    entry[ "name" ] = mCardDB[ cardId ][ "name" ];
  }

  QList< QVariantMap >* ref;
  if( player == PLAYER_SELF ) {
    ref = &mPlayerHistory;
  } else {
    ref = &mOpponentHistory;
  }

  *ref = entries.values();

  qSort( ref->begin(), ref->end(), []( const QVariantMap& a, const QVariantMap& b ) {
    if( a["mana"].toInt() == b["mana"].toInt() ) {
      return a["name"].toString() < b["name"].toString();
    } else {
      return a["mana"].toInt() < b["mana"].toInt();
    }
  });
}

void Overlay::HandleCardHistoryListUpdate( const ::CardHistoryList& cardHistoryList ) {
  UpdateHistoryFor( PLAYER_OPPONENT, cardHistoryList );
  update();
}

void Overlay::HandleCardDrawHistoryListUpdate( const ::CardHistoryList& cardDrawHistoryList ) {
  UpdateHistoryFor( PLAYER_SELF, cardDrawHistoryList );
  update();
}
