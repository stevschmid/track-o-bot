#pragma once

#include <QAction>
#include <QDialog>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMainWindow>
#include <QStatusBar>

#include "LogTab.h"
#include "AccountTab.h"
#include "SettingsTab.h"
#include "AboutTab.h"

namespace Ui { class MainWindow; }

#define NUM_TABS 4

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

  SettingsTab       *mSettingsTab;
  AccountTab        *mAccountTab;
  LogTab            *mLogTab;
  AboutTab          *mAboutTab;

  QWidget*          mTabs[ NUM_TABS ];

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
  void HandleGameClientRestartRequired();
  void HandleGameClientStop();

signals:
  void OpenProfile();

public:
  explicit Window();
  ~Window();
};

