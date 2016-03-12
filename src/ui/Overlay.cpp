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

  painter.setPen( Qt::red );
  painter.drawText( 0, 100, "Hello World" );
}

void Overlay::GameWindowChanged( int x, int y, int w, int h ) {
  LOG( "GameWindowChanged %d %d %d %d", x, y, w, h );
  move( x, y );
  setFixedSize( w, h );
  update();
}
