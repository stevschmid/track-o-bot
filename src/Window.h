#pragma once

#include <QDialog>
#include <QSystemTrayIcon>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QMainWindow>

#include "Core.h"
#include "Autostart.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMessageBox;
class QSystemTrayIcon;
class QTextEdit;
class QTabWidget;
class QLineEdit;
class QCheckBox;
class QLabel;
QT_END_NAMESPACE

#include "logger.h"

namespace Ui { class SettingsWidget; }

class SettingsTab : public QWidget
{
  Q_OBJECT

private:
  Ui::SettingsWidget *mUI;

private slots:
  void ExportAccount();
  void ImportAccount();
  void CheckForUpdatesNow();

public:
  explicit SettingsTab( QWidget *parent = 0 );
  ~SettingsTab();

public slots:
  void UpdateAutostart();
  void UpdateUpdateCheck();
  void LoadSettings();
};

namespace Ui { class LogWidget; }

class LogTab : public QWidget
{
  Q_OBJECT

private:
  Ui::LogWidget *mUI;

private slots:
  void AddLogEntry( LogEventType type, const string& message );

public:
  explicit LogTab( QWidget *parent = 0 );
  ~LogTab();
};

namespace Ui { class AboutWidget; }

class AboutTab : public QWidget
{
  Q_OBJECT

private:
  Ui::AboutWidget *mUI;

public:
  explicit AboutTab( QWidget *parent = 0 );
  ~AboutTab();
};

namespace Ui { class MainWindow; }

class Window : public QMainWindow
{
  Q_OBJECT

private:
  void closeEvent( QCloseEvent *event );
  void showEvent( QShowEvent *event );

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

  Core              mCore;

private slots:
  void RiseAndShine();
  void OpenProfile();
  void TrayIconActivated( QSystemTrayIcon::ActivationReason reason );

  void HandleGameClientRestartRequired( bool restartRequired );
  void HandleFirstStartCheck();

  void ActionTriggered( QAction *action );

public:
  explicit Window();
  ~Window();
};

