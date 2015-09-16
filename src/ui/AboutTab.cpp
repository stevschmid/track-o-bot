#include "AboutTab.h"
#include "ui_AboutWidget.h"

AboutTab::AboutTab( QWidget *parent )
  : QWidget( parent ), mUI( new Ui::AboutWidget )
{
  mUI->setupUi( this );
  mUI->aboutText->setHtml( mUI->aboutText->toHtml().replace( "VERSION", VERSION ) );
}

AboutTab::~AboutTab() {
  delete mUI;
}

