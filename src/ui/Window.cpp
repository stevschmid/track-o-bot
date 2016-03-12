#include "Window.h"

#include <QtGui>

#include "ui_MainWindow.h"

#include "Overlay.h"

#if defined Q_OS_MAC
#include "../OSXLocal.h"
#endif

#include "../Settings.h"
#include "../Hearthstone.h"

Window::Window()
  : mUI( new Ui::MainWindow )
{
  mUI->setupUi( this );
  setWindowFlags( (Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint) );

  setWindowTitle( qApp->applicationName() );

  CreateActions();
  CreateTrayIcon();

  connect( mTrayIcon, &QSystemTrayIcon::activated, this, &Window::TrayIconActivated );

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

  connect( group, &QActionGroup::triggered, this, &Window::ActionTriggered );
  connect( mUI->pageWidget, &QStackedWidget::currentChanged, this, &Window::TabChanged );

  connect( Hearthstone::Instance(), &Hearthstone::GameRequiresRestart, this, &Window::HandleGameClientRestartRequired );
  connect( Hearthstone::Instance(), &Hearthstone::GameStopped, this, &Window::HandleGameClientStop );

  QTimer::singleShot( 1000, this, &Window::HandleFirstStartCheck );

  new Overlay( this );

#ifdef Q_OS_WIN
  // This is a fix for
  // https://bugreports.qt.io/browse/QTBUG-35986
  // If the window was never opened, the WM_ENDSESSION event will
  // not be received by the app (tray icon will disappear
  // but the app will not close)
  // So show, then hide it at the beginning
  // With Qt:Tool the Window is not actually shown
  show();
  hide();
#endif
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
    OpenProfileRequested();
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
  connect( mOpenProfileAction, &QAction::triggered, this, &Window::OpenProfileRequested );

  mShowAction = new QAction( tr( "Settings..." ), this );
  connect( mShowAction, &QAction::triggered, this, &Window::RiseAndShine );

  mQuitAction = new QAction( tr("Quit"), this );
  connect( mQuitAction, &QAction::triggered, qApp, &QCoreApplication::quit );

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

void Window::OpenProfileRequested() {
  emit OpenProfile();
}

void Window::HandleGameClientRestartRequired() {
  mTrayIconMenu->insertAction( mOpenProfileAction, mGameClientRestartRequiredAction );
  ShowNotification( "Game log updated", "Please restart Hearthstone for changes to take effect!" );
}

void Window::HandleGameClientStop() {
  mTrayIconMenu->removeAction( mGameClientRestartRequiredAction );
}
