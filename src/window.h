#pragma once

#include <QSystemTrayIcon>
#include <QDialog>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMessageBox;
QT_END_NAMESPACE

class Window : public QDialog
{
  Q_OBJECT

public:
  Window();

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void riseAndShine();

private:
  void createActions();
  void createTrayIcon();

  QAction *showAction;
  QAction *quitAction;

  QSystemTrayIcon *trayIcon;
  QMenu *trayIconMenu;
};
