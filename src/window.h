#pragma once

#include <QDialog>

#include "core.h"
#include "autostart.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMessageBox;
class QSystemTrayIcon;
class QTextEdit;
class QTabWidget;
class QDialogButtonBox;
class QLineEdit;
class QCheckBox;
QT_END_NAMESPACE

#include "logger.h"

class SettingsTab : public QWidget
{
  Q_OBJECT

protected:
  QLineEdit *username;
  QLineEdit *password;
  QCheckBox *startAtLogin;

private slots:
  void reveal();
  void conceal();

public slots:
  void updateSettings();
  void applySettings();

public:
  SettingsTab(QWidget *parent = 0);
};

class LogTab;
class TabLogHandler : public LoggingObserver {
protected:
  LogTab *tab;

public:
  TabLogHandler(LogTab *window);
  void HandleLogEntry(const string& entry);
};

class LogTab : public QWidget
{
  Q_OBJECT

protected:
  QTextEdit *logText;
  TabLogHandler logHandler;

public:
  void addLogEntry(const string& entry);
  LogTab(QWidget *parent = 0);
  ~LogTab();
};

class Window : public QDialog
{
  Q_OBJECT

public:
  Window();
  ~Window();

protected:
  void closeEvent(QCloseEvent *event);
  void showEvent(QShowEvent *event);
  void reject();

private slots:
  void riseAndShine();
  void openProfile();

  void ok();
  void cancel();

private:
  void createActions();
  void createTrayIcon();

  QAction *showAction;
  QAction *quitAction;
  QAction *openProfileAction;

  QSystemTrayIcon *trayIcon;
  QMenu *trayIconMenu;

  QTabWidget *tabWidget;
  SettingsTab *settingsTab;
  LogTab *logTab;

  QDialogButtonBox *buttonBox;

  Core core;
};

