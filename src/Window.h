#pragma once

#include <QDialog>
#include <QSystemTrayIcon>

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
  Ui::SettingsWidget *ui;

private slots:
  void exportAccount();
  void importAccount();
  void checkForUpdatesNow();

public slots:
  void updateAutostart();
  void updateUpdateCheck();
  void loadSettings();

public:
  explicit SettingsTab(QWidget *parent = 0);
  ~SettingsTab();
};

namespace Ui { class LogWidget; }

class LogTab : public QWidget
{
  Q_OBJECT

private:
  Ui::LogWidget *ui;

public:
  explicit LogTab(QWidget *parent = 0);
  ~LogTab();

private slots:
  void addLogEntry(const string& message);
};

namespace Ui { class AboutWidget; }

class AboutTab : public QWidget
{
  Q_OBJECT

private:
  Ui::AboutWidget *ui;

public:
  explicit AboutTab(QWidget *parent = 0);
  ~AboutTab();

};

namespace Ui { class Window; }

class Window : public QDialog
{
  Q_OBJECT

public:
  explicit Window();
  ~Window();

private slots:
  void riseAndShine();
  void openProfile();
  void trayIconActivated(QSystemTrayIcon::ActivationReason);

private:
  void closeEvent(QCloseEvent *event);
  void showEvent(QShowEvent *event);
  void reject();

  void createActions();
  void createTrayIcon();

  Ui::Window *ui;

  QAction *showAction;
  QAction *quitAction;
  QAction *openProfileAction;

  QSystemTrayIcon *trayIcon;
  QMenu *trayIconMenu;

  Core core;
};

