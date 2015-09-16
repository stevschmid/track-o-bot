#pragma once

#include <QAction>
#include <QDialog>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMainWindow>
#include <QStatusBar>

namespace Ui { class MainWindow; }

class Window : public QMainWindow
{
  Q_OBJECT

private:
  void closeEvent( QCloseEvent *event );

  void CreateActions();
  void CreateTrayIcon();

  void ShowNotification( const char *title, const char *message );

  Ui::MainWindow    *mUI;

  QAction           *mShowAction;
  QAction           *mQuitAction;
  QAction           *mOpenProfileAction;
  QAction           *mGameClientRestartRequiredAction;

  QSystemTrayIcon   *mTrayIcon;
  QMenu             *mTrayIconMenu;

private slots:
  void RiseAndShine();
  void OpenProfileRequested();
  void TrayIconActivated( QSystemTrayIcon::ActivationReason reason );

  void HandleFirstStartCheck();

  void ActionTriggered( QAction *action );
  void TabChanged( int index );

public slots:
  void HandleGameClientRestartRequired( bool restartRequired );

signals:
  void OpenProfile();

public:
  explicit Window();
  ~Window();
};

