#pragma once

#include <QAction>
#include <QDialog>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMainWindow>

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
  void OpenProfile();
  void TrayIconActivated( QSystemTrayIcon::ActivationReason reason );

  void HandleFirstStartCheck();

  void ActionTriggered( QAction *action );
  void TabChanged( int index );

public slots:
  void HandleGameClientRestartRequired( bool restartRequired );

public:
  explicit Window();
  ~Window();
};

