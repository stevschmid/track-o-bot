#include "Window.h"

#include <QtGui>

#include "ui_Window.h"
#include "ui_SettingsWidget.h"
#include "ui_AccountWidget.h"
#include "ui_LogWidget.h"
#include "ui_AboutWidget.h"
#include "ui_MainWindow.h"

#if defined Q_OS_MAC
#include "OSXLocal.h"
#endif

#include "Settings.h"

SettingsTab::SettingsTab( QWidget *parent )
  : QWidget( parent ), mUI( new Ui::SettingsWidget )
{
  mUI->setupUi( this );
  connect( mUI->checkForUpdatesNowButton, SIGNAL( clicked() ), this, SLOT( CheckForUpdatesNow() ) );
  connect( mUI->startAtLogin, SIGNAL( clicked(bool) ), this, SLOT( UpdateAutostart() ) );
  connect( mUI->checkForUpdates, SIGNAL( clicked(bool) ), this, SLOT( UpdateAutoUpdateCheck() ) );
  connect( mUI->replaysEnabled, SIGNAL( clicked(bool) ), this, SLOT( UpdateReplaysEnabled() ) );
  LoadSettings();
}

SettingsTab::~SettingsTab() {
  delete mUI;
}

void SettingsTab::CheckForUpdatesNow() {
  Settings::Instance()->CheckForUpdates();
}

void SettingsTab::UpdateAutostart() {
  Settings::Instance()->SetAutostart( mUI->startAtLogin->isChecked() );
}

void SettingsTab::UpdateAutoUpdateCheck() {
  Settings::Instance()->SetAutoUpdateCheck( mUI->checkForUpdates->isChecked() );
}

void SettingsTab::UpdateReplaysEnabled() {
  Settings::Instance()->SetReplaysEnabled( mUI->replaysEnabled->isChecked() );
}

void SettingsTab::LoadSettings() {
  Settings *settings = Settings::Instance();

  mUI->startAtLogin->setChecked( settings->Autostart() );
  mUI->checkForUpdates->setChecked( settings->AutoUpdateCheck() );

  bool replayRequirements = settings->ReplayRequirementsFulfilled();
  bool replaysEnabled = replayRequirements && settings->ReplaysEnabled();
  mUI->replaysEnabled->setEnabled( replayRequirements );
  mUI->replaysEnabled->setChecked( replaysEnabled );
}

AccountTab::AccountTab( QWidget *parent )
  : QWidget( parent ), mUI( new Ui::AccountWidget )
{
  mUI->setupUi( this );
  connect( mUI->exportAccountButton, SIGNAL( clicked() ), this, SLOT( ExportAccount() ) );
  connect( mUI->importAccountButton, SIGNAL( clicked() ), this, SLOT( ImportAccount() ) );
  connect( Settings::Instance(), SIGNAL( AccountChanged(const QString&, const QString&) ), this, SLOT( LoadAccount() ) );
  LoadAccount();
}

AccountTab::~AccountTab() {
  delete mUI;
}

void AccountTab::LoadAccount() {
  Settings *settings = Settings::Instance();

  bool accountSetUp = settings->HasAccount();
  if( accountSetUp ) {
    mUI->account->setText( settings->AccountUsername() );
  }

  mUI->importAccountButton->setEnabled( accountSetUp );
  mUI->exportAccountButton->setEnabled( accountSetUp );
}

void AccountTab::ExportAccount() {
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

    Settings *settings = Settings::Instance();

    QDataStream out( &file );
    out.setVersion( QDataStream::Qt_4_8 );
    out << settings->AccountUsername();
    out << settings->AccountPassword();
    out << settings->WebserviceURL();

    LOG( "Account %s exported in %s", qt2cstr( settings->AccountUsername() ), qt2cstr( fileName ) );
  }
}

void AccountTab::ImportAccount() {
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
      Settings::Instance()->SetAccount( username, password );
      Settings::Instance()->SetWebserviceURL( webserviceUrl );

      LOG( "Account %s imported from %s", qt2cstr( username ), qt2cstr( fileName ) );
      LoadAccount();
    } else {
      ERR( "Import failed" );
    }
  }
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
  : mUI( new Ui::MainWindow )
{
  mUI->setupUi( this );
  setWindowFlags( (Qt::Window | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint) );

  setWindowTitle( qApp->applicationName() );

  CreateActions();
  CreateTrayIcon();

  connect( mTrayIcon, SIGNAL( activated(QSystemTrayIcon::ActivationReason) ), this, SLOT( TrayIconActivated(QSystemTrayIcon::ActivationReason) ) );
  connect( &mCore, SIGNAL( HandleGameClientRestartRequired(bool) ), this, SLOT( HandleGameClientRestartRequired(bool) ) );

  QActionGroup *group = new QActionGroup( this );
  group->setExclusive( true );

  mUI->actionSettings->setActionGroup( group );
  mUI->actionSettings->setProperty( "pageIndex", 0 );

  mUI->actionAccount->setActionGroup( group );
  mUI->actionAccount->setProperty( "pageIndex", 1 );

  mUI->actionLog->setActionGroup( group );
  mUI->actionLog->setProperty( "pageIndex", 2 );

  mUI->actionAbout->setActionGroup( group );
  mUI->actionAbout->setProperty( "pageIndex", 3 );

  mUI->pageWidget->setCurrentIndex( 0 );
  mUI->actionSettings->setChecked( true );
  TabChanged( 0 );

  connect( group, SIGNAL( triggered(QAction*) ), this, SLOT( ActionTriggered(QAction*) ) );
  connect( mUI->pageWidget, SIGNAL( currentChanged(int) ), this, SLOT( TabChanged( int ) ) );

  QTimer::singleShot( 1000, this, SLOT(HandleFirstStartCheck()) );
}

Window::~Window() {
  delete mUI;
}

void Window::ActionTriggered( QAction *action ) {
  int page = action->property( "pageIndex" ).toInt();
  mUI->pageWidget->setCurrentIndex( page );
}

void Window::TabChanged( int index ) {
  QWidget *widget = mUI->pageWidget->widget( index );
  mUI->pageWidget->setFixedHeight( widget->minimumHeight() );
  adjustSize();
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
    OpenProfile();
  }
#else
  UNUSED_ARG( reason );
#endif
}

void Window::closeEvent( QCloseEvent *event ) {
  if( mTrayIcon->isVisible() ) {
    hide();
    event->ignore();
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
  Settings::Instance()->OpenProfile();
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
