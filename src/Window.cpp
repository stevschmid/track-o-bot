#include <QtGui>
#include "Window.h"

#include "ui_Window.h"
#include "ui_SettingsWidget.h"
#include "ui_LogWidget.h"
#include "ui_AboutWidget.h"

#include "Tracker.h"
#include "Updater.h"
extern Updater *gUpdater;

#if defined Q_OS_MAC
#include "OSXLocal.h"
#endif

SettingsTab::SettingsTab( QWidget *parent )
  : QWidget( parent ), mUI( new Ui::SettingsWidget )
{
  mUI->setupUi( this );
  connect( mUI->exportAccountButton, SIGNAL( clicked() ), this, SLOT( ExportAccount() ) );
  connect( mUI->importAccountButton, SIGNAL( clicked() ), this, SLOT( ImportAccount() ) );
  connect( mUI->checkForUpdatesNowButton, SIGNAL( clicked()), this, SLOT( CheckForUpdatesNow() ) );
  connect( mUI->startAtLogin, SIGNAL( clicked(bool) ), this, SLOT( UpdateAutostart() ) );
  connect( mUI->checkForUpdates, SIGNAL( clicked(bool) ), this, SLOT( UpdateUpdateCheck() ) );
  connect( Tracker::Instance(), SIGNAL( AccountCreated() ), this, SLOT( LoadSettings() ) );
  LoadSettings();
}

SettingsTab::~SettingsTab() {
  delete mUI;
}

void SettingsTab::ExportAccount() {
  QString fileName = QFileDialog::getSaveFileName( this,
      tr( "Export Track-o-Bot Account Data" ), "",
      tr( "Account Data (*.track-o-bot);; All Files (*)" ) );

  if( fileName.isEmpty() ) {
    return;
  } else {
    QFile file( fileName );
    if( !file.open( QIODevice::WriteOnly ) ) {
      QMessageBox::information( this, tr( "Unable to open file" ), file.errorString() );
      return;
    }

    QDataStream out( &file );
    out.setVersion( QDataStream::Qt_4_8 );
    out << Tracker::Instance()->Username();
    out << Tracker::Instance()->Password();
    out << Tracker::Instance()->WebserviceURL();

    LOG( "Account %s exported in %s", Tracker::Instance()->Username().toStdString().c_str(), fileName.toStdString().c_str() );
  }
}

void SettingsTab::CheckForUpdatesNow() {
  if( gUpdater ) {
    gUpdater->CheckForUpdatesNow();
  }
}

void SettingsTab::ImportAccount() {
  QString fileName = QFileDialog::getOpenFileName( this,
      tr( "Import Track-o-Bot Account Data" ), "",
      tr( "Account Data (*.track-o-bot);; All Files (*)" ) );

  if( fileName.isEmpty() ) {
    return;
  } else {
    QFile file( fileName );
    if( !file.open( QIODevice::ReadOnly ) ) {
      QMessageBox::information( this, tr( "Unable to open file" ), file.errorString() );
      return;
    }

    QDataStream in( &file );
    QString username, password, webserviceUrl;
    in.setVersion( QDataStream::Qt_4_8 );
    in >> username;
    in >> password;
    in >> webserviceUrl;

    if( !username.isEmpty() && !password.isEmpty() && !webserviceUrl.isEmpty() ) {
      Tracker::Instance()->SetUsername( username );
      Tracker::Instance()->SetPassword( password );
      Tracker::Instance()->SetWebserviceURL( webserviceUrl );

      LOG( "Account %s imported from %s", username.toStdString().c_str(), fileName.toStdString().c_str() );
      LoadSettings();
    } else {
      ERR( "Import failed" );
    }
  }
}

void SettingsTab::UpdateAutostart() {
  Autostart autostart;
  autostart.SetActive( mUI->startAtLogin->isChecked() );
}

void SettingsTab::UpdateUpdateCheck() {
  if( gUpdater ) {
    gUpdater->SetAutomaticallyChecksForUpdates( mUI->checkForUpdates->isChecked() );
  }
}

void SettingsTab::LoadSettings() {
  Autostart autostart;
  mUI->startAtLogin->setChecked( autostart.Active() );

  if( gUpdater ) {
    mUI->checkForUpdates->setChecked( gUpdater->AutomaticallyChecksForUpdates() );
  }

  bool accountSetUp = Tracker::Instance()->IsAccountSetUp();
  if( accountSetUp ) {
    mUI->account->setText( Tracker::Instance()->Username() );
  }

  mUI->importAccountButton->setEnabled( accountSetUp );
  mUI->exportAccountButton->setEnabled( accountSetUp );
}

LogTab::LogTab( QWidget *parent )
  : QWidget( parent ), mUI( new Ui::LogWidget )
{
  mUI->setupUi( this );

  QFont fixedFont = QFontDatabase::systemFont( QFontDatabase::FixedFont );
  mUI->logText->setFont( fixedFont );

  connect( Logger::Instance(), SIGNAL( NewMessage(LogEventType, const string&) ), this, SLOT( AddLogEntry(LogEventType, const string&) ) );
}

LogTab::~LogTab() {
  delete mUI;
}

void LogTab::AddLogEntry( LogEventType type, const string& msg ) {
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

  mUI->logText->insertPlainText( msg.c_str() );
  mUI->logText->moveCursor( QTextCursor::End );
}

AboutTab::AboutTab( QWidget *parent )
  : QWidget( parent ), mUI( new Ui::AboutWidget )
{
  mUI->setupUi( this );

  QPixmap logoImage( ":/icons/logo.png" );
  mUI->logo->setPixmap( logoImage.scaled( 64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
  mUI->version->setText( VERSION );
}

AboutTab::~AboutTab() {
  delete mUI;
}

Window::Window()
  : mUI( new Ui::Window )
{
  mUI->setupUi( this );

  setWindowTitle( qApp->applicationName() );

  CreateActions();
  CreateTrayIcon();

  connect( mTrayIcon, SIGNAL( activated(QSystemTrayIcon::ActivationReason) ), this, SLOT( TrayIconActivated(QSystemTrayIcon::ActivationReason) ) );
  connect( &mCore, SIGNAL( HandleGameClientRestartRequired(bool) ), this, SLOT( HandleGameClientRestartRequired(bool) ) );

  QTimer::singleShot( 1000, this, SLOT(HandleFirstStartCheck()) );
}

Window::~Window() {
  delete mUI;
}

void Window::ShowNotification( const char *title, const char *message ) {
#if defined Q_OS_WIN
  mTrayIcon->showMessage( title, message );
#elif defined Q_OS_MAC
  OSX_ShowNotification( title, message );
#endif
}

void Window::HandleFirstStartCheck() {
  // Notify user the first time that the app runs in the taskbar
  QSettings settings;
  if( !settings.contains("taskbarHint") ) {
    settings.setValue( "taskbarHint", true );
#if defined Q_OS_WIN
    ShowNotification( "Heads up!", "Track-o-Bot runs in your taskbar! Right click the icon for more options." );
#elif defined Q_OS_MAC
    ShowNotification( "Track-o-Bot", "Track-o-Bot runs in your menu bar! Click the icon for more options." );
#endif
  }
}

void Window::TrayIconActivated( QSystemTrayIcon::ActivationReason reason ) {
#ifdef Q_OS_WIN
  if( reason == QSystemTrayIcon::ActivationReason::DoubleClick ) {
    Tracker::Instance()->OpenProfile();
  }
#endif
}

void Window::showEvent( QShowEvent *event ) {
  QDialog::showEvent( event );
  mUI->settingsTab->LoadSettings();
}

void Window::closeEvent( QCloseEvent *event ) {
  if( mTrayIcon->isVisible() ) {
    hide();
    event->ignore();
  }
}

// prevent esc from closing the app
void Window::reject() {
  if( mTrayIcon->isVisible() ) {
    hide();
  } else {
    QDialog::reject();
  }
}

void Window::CreateActions() {
  mOpenProfileAction = new QAction( tr( "Open Profile..." ), this );
  connect( mOpenProfileAction, SIGNAL( triggered() ), this, SLOT( OpenProfile() ) );

  mShowAction = new QAction( tr( "Settings..." ), this );
  connect( mShowAction, SIGNAL( triggered() ), this, SLOT( RiseAndShine() ) );

  mQuitAction = new QAction( tr("Quit"), this );
  connect( mQuitAction, SIGNAL( triggered() ), qApp, SLOT( quit() ) );

  mGameClientRestartRequiredAction = new QAction( tr("Please restart Hearthstone!" ), this );
  mGameClientRestartRequiredAction->setEnabled( false );
}

void Window::CreateTrayIcon() {
  mTrayIconMenu = new QMenu( this);
  mTrayIconMenu->addAction( mOpenProfileAction );
  mTrayIconMenu->addSeparator();
  mTrayIconMenu->addAction( mShowAction );
  mTrayIconMenu->addSeparator();
  mTrayIconMenu->addAction( mQuitAction );

  mTrayIcon = new QSystemTrayIcon( this );
  mTrayIcon->setContextMenu (mTrayIconMenu );

#if defined Q_OS_MAC
  QIcon::Mode blackMode = QIcon::Normal;
  QIcon::Mode whiteMode = QIcon::Selected;
  if( OSX_YosemiteDarkModeEnabled() ) {
    blackMode = QIcon::Disabled;
    whiteMode = QIcon::Normal;
  }

  QIcon icon;
  icon.addFile( ":/icons/mac_black@2x.png", QSize(), blackMode );
  icon.addFile( ":/icons/mac_black.png", QSize(), blackMode );
  icon.addFile( ":/icons/mac_white.png", QSize(), whiteMode );
  icon.addFile( ":/icons/mac_white@2x.png", QSize(), whiteMode );
#elif defined Q_OS_WIN
  QIcon icon = QIcon( ":/icons/win.ico" );
#endif

  mTrayIcon->setIcon( icon );
  mTrayIcon->show();
}

void Window::RiseAndShine() {
  show();
  raise();
}

void Window::OpenProfile() {
  Tracker::Instance()->OpenProfile();
}

void Window::HandleGameClientRestartRequired( bool restartRequired ) {
  static QAction *separator = NULL;

  if( restartRequired ) {
    separator = mTrayIconMenu->insertSeparator( mOpenProfileAction );
    mTrayIconMenu->insertAction( separator, mGameClientRestartRequiredAction );

    ShowNotification( "Game log enabled", "Please restart Hearthstone for changes to take effect!" );
  } else {
    mTrayIconMenu->removeAction( mGameClientRestartRequiredAction );
    if( separator ) {
      mTrayIconMenu->removeAction( separator );
    }
  }
}
