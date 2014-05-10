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

  void ok();
  void cancel();

public slots:
  void updateSettings();
  void applySettings();

public:
  explicit SettingsTab(QWidget *parent = 0);
};

class LogTab : public QWidget
{
  Q_OBJECT

protected:
  QTextEdit *logText;

public:
  explicit LogTab(QWidget *parent = 0);

private slots:
  void addLogEntry(const string& message);
};

class AboutTab : public QWidget
{
  Q_OBJECT

public:
  explicit AboutTab(QWidget *parent = 0);
};

class Window : public QDialog
{
  Q_OBJECT

public:
  explicit Window();
  ~Window();

protected:
  void closeEvent(QCloseEvent *event);
  void showEvent(QShowEvent *event);
  void reject();

private slots:
  void riseAndShine();
  void openProfile();

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
  AboutTab *aboutTab;

  Core core;
};

