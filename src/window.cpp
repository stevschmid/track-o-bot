#include <QtGui>
#include "window.h"

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

void Window::riseAndShine() {
  raise();
  show();
}

void Window::createActions()
{
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
  trayIconMenu->addAction(quitAction);

  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setContextMenu(trayIconMenu);

  QIcon icon = QIcon(":/icons/heart.svg");
  trayIcon->setIcon(icon);
  setWindowIcon(icon);
}

void Window::addLogEntry(const string& entry) {
  logText->moveCursor (QTextCursor::End);
  logText->insertPlainText(entry.c_str());
  logText->moveCursor (QTextCursor::End);
}
