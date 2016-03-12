#include "Overlay.h"
#include "ui_Overlay.h"


#include "../Hearthstone.h"

#include <objc/objc-runtime.h>

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

  connect( Hearthstone::Instance(), &Hearthstone::GameWindowChanged, this, &Overlay::GameWindowChanged );

#ifdef Q_OS_MAC
  WId windowObject = this->winId();
  objc_object* nsviewObject = reinterpret_cast<objc_object*>(windowObject);
  objc_object* nsWindowObject = objc_msgSend( nsviewObject, sel_registerName("window") );
  int NSWindowCollectionBehaviorCanJoinAllSpaces = 1 << 0;
  objc_msgSend( nsWindowObject, sel_registerName("setCollectionBehavior:"), NSWindowCollectionBehaviorCanJoinAllSpaces );
#endif

  show();
}

Overlay::~Overlay() {
  delete mUI;
}

void Overlay::paintEvent( QPaintEvent* ) {
  QPainter painter(this);

  QFontMetrics metrics(font());

  int player = 100, opponent = 100;
  int spacing = metrics.ascent() - metrics.descent();

  QPen w(QColor(0xFF, 0xFF, 0xFF, 0x80));
  painter.setPen( w );

  for( const CardHistoryItem& it : mCardHistoryList ) {
    if( it.player == PLAYER_SELF ) {
      painter.drawText( 0.8 * width(), player, it.cardId.c_str() );
      player += spacing;
    } else {
      painter.drawText( 0.1 * width(), opponent, it.cardId.c_str() );
      opponent += spacing;
    }
  }
}

void Overlay::GameWindowChanged( int x, int y, int w, int h ) {
  LOG( "GameWindowChanged %d %d %d %d", x, y, w, h );
  move( x, y );
  setFixedSize( w, h );
  update();
}

void Overlay::HandleCardHistoryListUpdate( const ::CardHistoryList& cardHistoryList ) {
  mCardHistoryList = cardHistoryList;
  update();
}
