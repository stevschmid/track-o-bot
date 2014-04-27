#include <QtGui>
#include "window.h"

#include <QVBoxLayout>
#include <QTextEdit>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QAction>
#include <QMenu>

WindowLogHandler::WindowLogHandler(Window *window)
  :window(window)
{
}

void WindowLogHandler::HandleLogEntry(const string& entry) {
  window->addLogEntry(entry);
}

Window::Window()
  :logHandler(this)
{
  setWindowTitle("Arenatracker");

  Qt::WindowFlags flags = windowFlags();
  flags |= Qt::CustomizeWindowHint;
  flags &= ~Qt::WindowMaximizeButtonHint;
  setWindowFlags(flags);

  createActions();
  createTrayIcon();

  QVBoxLayout *mainLayout = new QVBoxLayout;
  setLayout(mainLayout);

  logText = new QTextEdit;
  mainLayout->addWidget(logText);

  trayIcon->show();

  setFixedSize(400, 300);

  logger.RegisterObserver(&logHandler);
}

Window::~Window() {
  logger.UnregisterObserver(&logHandler);
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
  autostartAction = new QAction(tr("Launch at Login"), this);
  connect(autostartAction, SIGNAL(triggered()), this, SLOT(updateAutostart()));
  autostartAction->setCheckable(true);
  autostartAction->setChecked(autostart.IsActive());

  showAction = new QAction(tr("Show"), this);
  connect(showAction, SIGNAL(triggered()), this, SLOT(riseAndShine()));

  quitAction = new QAction(tr("Quit"), this);
  connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void Window::createTrayIcon()
{
  trayIconMenu = new QMenu(this);
  trayIconMenu->addAction(showAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(autostartAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(quitAction);

  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setContextMenu(trayIconMenu);

  QIcon icon = QIcon(":/icons/paw.svg");
  trayIcon->setIcon(icon);
  setWindowIcon(icon);
}

void Window::addLogEntry(const string& entry) {
  logText->moveCursor (QTextCursor::End);
  logText->insertPlainText(entry.c_str());
  logText->moveCursor (QTextCursor::End);
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
