#include <QtGui>
#include "window.h"

#include <QVBoxLayout>
#include <QTextEdit>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QAction>
#include <QMenu>
#include <QTabWidget>

TabLogHandler::TabLogHandler(LogTab *logTab)
  :tab(logTab)
{
}

void TabLogHandler::HandleLogEntry(const string& entry) {
  tab->addLogEntry(entry);
}

SettingsTab::SettingsTab(QWidget *parent)
  : QWidget(parent)
{
}

LogTab::LogTab(QWidget *parent)
  : QWidget(parent), logHandler(this)
{
  QVBoxLayout *layout = new QVBoxLayout;
  logText = new QTextEdit;
  layout->setSizeConstraint(QLayout::SetNoConstraint);
  layout->addWidget(logText);
  setLayout(layout);

  logger.RegisterObserver(&logHandler);
}

LogTab::~LogTab() {
  logger.UnregisterObserver(&logHandler);
}

void LogTab::addLogEntry(const string& entry) {
  logText->moveCursor (QTextCursor::End);
  logText->insertPlainText(entry.c_str());
  logText->moveCursor (QTextCursor::End);
}

Window::Window()
{
  setWindowTitle(APP_NAME);

  Qt::WindowFlags flags = windowFlags();
  flags |= Qt::CustomizeWindowHint;
  flags &= ~Qt::WindowMaximizeButtonHint;
  setWindowFlags(flags);

  createActions();
  createTrayIcon();

  tabWidget = new QTabWidget;
  tabWidget->addTab(new SettingsTab, tr("Settings"));
  tabWidget->addTab(new LogTab, tr("Log"));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
  mainLayout->addWidget(tabWidget);
  setLayout(mainLayout);

  setFixedSize(400, 300);
}

Window::~Window() {
}

void Window::closeEvent(QCloseEvent *event)
{
  if(trayIcon->isVisible()) {
    hide();
    event->ignore();
  }
}

void Window::createActions()
{
  openProfileAction = new QAction(tr("Open Profile..."), this);
  connect(openProfileAction, SIGNAL(triggered()), this, SLOT(openProfile()));

  autostartAction = new QAction(tr("Launch at Login"), this);
  connect(autostartAction, SIGNAL(triggered()), this, SLOT(updateAutostart()));
  autostartAction->setCheckable(true);
  autostartAction->setChecked(autostart.IsActive());

  showAction = new QAction(tr("Show..."), this);
  connect(showAction, SIGNAL(triggered()), this, SLOT(riseAndShine()));

  quitAction = new QAction(tr("Quit"), this);
  connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void Window::createTrayIcon()
{
  trayIconMenu = new QMenu(this);
  trayIconMenu->addAction(openProfileAction);
  trayIconMenu->addAction(showAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(autostartAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(quitAction);

  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setContextMenu(trayIconMenu);

  QIcon icon = QIcon(":/icons/paw.svg");
  trayIcon->setIcon(icon);
  trayIcon->show();
}

void Window::riseAndShine() {
  show();
  raise();
}

void Window::updateAutostart() {
  if(autostartAction->isChecked()) {
    autostart.SetActive(true);
  } else {
    autostart.SetActive(false);
  }
}

void Window::openProfile() {
  core.Tracker().OpenProfile();
}
