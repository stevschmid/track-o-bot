#include <QtGui>
#include "window.h"

Window::Window()
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

  trayIcon->show();

  setFixedSize(400, 300);
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
