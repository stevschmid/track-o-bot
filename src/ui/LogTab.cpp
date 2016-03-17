#include "LogTab.h"
#include "ui_LogWidget.h"

#include "../Logger.h"

#include <QFontDatabase>

LogTab::LogTab( QWidget *parent )
  : QWidget( parent ), mUI( new Ui::LogWidget )
{
  mUI->setupUi( this );

  QFont fixedFont = QFontDatabase::systemFont( QFontDatabase::FixedFont );
  mUI->logText->setFont( fixedFont );

  connect( Logger::Instance(), &Logger::NewMessage, this, &LogTab::AddLogEntry );
}


LogTab::~LogTab() {
  delete mUI;
}

void LogTab::AddLogEntry( LogEventType type, const QString& msg ) {
  UNUSED_ARG( type );

  mUI->logText->appendPlainText( msg.trimmed() );
  mUI->logText->moveCursor( QTextCursor::End );
}

