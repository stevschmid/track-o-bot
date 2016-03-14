#include "Overlay.h"
#include "ui_Overlay.h"

#include "../Hearthstone.h"

#include <objc/objc-runtime.h>
#include <cassert>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QMouseEvent>

#define CHECK_FOR_OVERLAY_HOVER_INTERVAL_MS 100

Overlay::Overlay( QWidget *parent )
  : QMainWindow( parent ), mUI( new Ui::Overlay ), mShowPlayerHistory( PLAYER_UNKNOWN )
{
  mUI->setupUi( this );
  setWindowFlags( Qt::NoDropShadowWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Window );
  setAttribute( Qt::WA_TranslucentBackground );

  connect( Hearthstone::Instance(), &Hearthstone::GameWindowChanged, this, &Overlay::HandleGameWindowChanged );
  connect( Hearthstone::Instance(), &Hearthstone::GameStarted, this, &Overlay::HandleGameStarted );
  connect( Hearthstone::Instance(), &Hearthstone::GameStopped, this, &Overlay::HandleGameStopped );

  connect( &mCheckForHoverTimer, &QTimer::timeout, this, &Overlay::CheckForHover );

  LoadCards();

  hide();

#ifdef Q_OS_MAC
  WId windowObject = this->winId();
  objc_object* nsviewObject = reinterpret_cast<objc_object*>(windowObject);
  objc_object* nsWindowObject = objc_msgSend( nsviewObject, sel_registerName("window") );
  int NSWindowCollectionBehaviorCanJoinAllSpaces = 1 << 0;
  objc_msgSend( nsWindowObject, sel_registerName("setCollectionBehavior:"), NSWindowCollectionBehaviorCanJoinAllSpaces );
#endif
}

Overlay::~Overlay() {
  delete mUI;
}

void Overlay::CheckForHover() {
  QPoint mouseLoc = QCursor::pos();

  Player showPlayerHistory = PLAYER_UNKNOWN;

  if( mPlayerDeckRect.contains( mouseLoc ) ) {
    showPlayerHistory = PLAYER_SELF;
  } else if( mOpponentDeckRect.contains( mouseLoc ) ) {
    showPlayerHistory = PLAYER_OPPONENT;
  }

  LOG( "%d", showPlayerHistory );

  if( mShowPlayerHistory != showPlayerHistory ) {
    mShowPlayerHistory = showPlayerHistory;
    update();
  }
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

void DrawMana( QPainter& painter, int x, int y, int width, int height, int mana ) {
  // Draw mana
  QPen origPen = painter.pen();
  QPen pen( QColor( 0, 52, 113 ) );
  pen.setCosmetic( true );
  pen.setWidth( 1 );
  painter.setPen( pen );

  QBrush brush( QColor( 40, 119, 238 ) );
  painter.setBrush( brush );

  QTransform transform;
  painter.translate( x + width * 0.5, y + height * 0.5 );
  painter.scale( width * 0.8, height * 0.8 );

  static const QPointF points[5] = {
    QPointF( 0.0, -1.0 ),
    QPointF( 1.0, -0.2 ),
    QPointF( 0.6, 1.0 ),
    QPointF( -0.6, 1.0 ),
    QPointF( -1.0, -0.2 ),
  };
  painter.drawConvexPolygon( points, 5 );
  painter.resetTransform();
  painter.setPen( origPen );

  painter.drawText( x, y, width, height, Qt::AlignCenter | Qt::AlignVCenter, QString::number( mana ) );
}

void DrawCardLine( QPainter& painter, int x, int y, int width, int height, const QString& name, int count ) {
  QString line = name;
  if( count > 1 ) {
    line += QString( " (x%1)" ).arg( count );
  }

  painter.drawText( x, y, width, height, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip, line );
  painter.resetTransform();
}

void Overlay::paintEvent( QPaintEvent* ) {
  QPainter painter( this );
  painter.setRenderHint( QPainter::Antialiasing );

  CardHistoryList list;
  list.push_back( CardHistoryItem( 0, PLAYER_SELF, "CS_013" ) );
  list.push_back( CardHistoryItem( 0, PLAYER_SELF, "EX1_166" ) );
  list.push_back( CardHistoryItem( 0, PLAYER_SELF, "CS2_012" ) );
  list.push_back( CardHistoryItem( 0, PLAYER_SELF, "EX1_571" ) );
  list.push_back( CardHistoryItem( 0, PLAYER_SELF, "EX1_571" ) );
  list.push_back( CardHistoryItem( 0, PLAYER_SELF, "NEW1_008" ) );
  UpdateHistoryFor( PLAYER_SELF, list );

  if( mShowPlayerHistory == PLAYER_SELF ) {
    /* PaintHistory( painter, mPlayerDeckRect.x(), mPlayerDeckRect.y(), 200, "Cards drawn", mPlayerHistory ); */

    PaintHistory( painter, width() - 200, 0.15 * height(), 200, "Cards drawn", mPlayerHistory );
  } else if( mShowPlayerHistory == PLAYER_OPPONENT ) {
    PaintHistory( painter, 20, 0.15 * height(), 200, "Cards played by opponent", mOpponentHistory );
  }
}

void Overlay::PaintHistory( QPainter& painter, int x, int y, int width, const QString& title, QList< QVariantMap >& history ) {
  int padding = 10;

  QFont rowFont = font();
  rowFont.setPointSize( 12 );

  QFont titleFont = font();
  titleFont.setPointSize( 12 );
  titleFont.setUnderline( true );
  titleFont.setBold( true );

  QFontMetrics rowMetrics( rowFont );
  int rowHeight = rowMetrics.ascent() - rowMetrics.descent();
  int rowSpacing = 10;
  int rowWidth = width - padding * 2;

  QFontMetrics titleMetrics( titleFont );
  int titleHeight = titleMetrics.ascent() - titleMetrics.descent();
  int totalHeight = history.count() * ( rowHeight + rowSpacing ) + rowSpacing + ( titleHeight + rowSpacing );

  // Rect
  QRect rect( x, y, width, totalHeight );
  painter.setClipRect( rect );

  QPen pen = QPen( QColor( 160, 160, 160 ) );
  pen.setWidth( 3 );
  painter.setPen( pen );
  painter.setBrush( QBrush( QColor( 70, 70, 70 ) ) );
  painter.drawRoundedRect( rect, 10, 10 );

  // Title
  y += padding;
  painter.setPen( QPen( Qt::white) );
  painter.setFont( titleFont );
  painter.drawText( x, y, width, titleHeight, Qt::AlignCenter | Qt::AlignVCenter | Qt::TextDontClip, title );
  y += titleHeight + rowSpacing;

  // Lines
  painter.setFont( rowFont );
  for( const QVariantMap& it : history ) {
    int mx = x + padding;
    DrawMana( painter, mx, y, rowHeight, rowHeight, it["mana"].toInt() );
    int cx = mx + rowHeight + 5;
    DrawCardLine( painter, cx, y, rowWidth - cx, rowHeight, it["name"].toString(), it["count"].toInt() );
    y += rowHeight + rowSpacing;
  }
}

void Overlay::HandleGameWindowChanged( int x, int y, int w, int h ) {
  LOG( "HandleGameWindowChanged %d %d %d %d", x, y, w, h );

  move( x, y );
  setFixedSize( w, h );

  int minWidth = h * 4 / 3;
  QRect localPlayerDeck( w / 2 + 0.435 * minWidth, h * 0.570, 0.05 * minWidth, h * 0.135 );
  QRect localOpponentDeck = localPlayerDeck.translated( 0, -0.27 * h );

  mPlayerDeckRect = localPlayerDeck.translated( x, y );
  mOpponentDeckRect = localOpponentDeck.translated( x, y );

  update();
}

void Overlay::HandleGameStarted() {
  show();
  mCheckForHoverTimer.start( CHECK_FOR_OVERLAY_HOVER_INTERVAL_MS );
}

void Overlay::HandleGameStopped() {
  hide();
  mCheckForHoverTimer.stop();
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
