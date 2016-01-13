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
  mUI->logText->moveCursor( QTextCursor::End );

  switch( type ) {
    case LOG_ERROR:
      mUI->logText->setTextColor( Qt::red );
      break;

    case LOG_DEBUG:
      mUI->logText->setTextColor( Qt::gray );
      break;

    default:
      mUI->logText->setTextColor( QApplication::palette().text().color() );
  }

  mUI->logText->insertPlainText( msg );
  mUI->logText->moveCursor( QTextCursor::End );
}

