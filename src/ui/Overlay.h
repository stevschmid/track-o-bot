#pragma once

#include <QMainWindow>

namespace Ui { class Overlay; }

class Overlay : public QMainWindow
{
  Q_OBJECT

private:
  Ui::Overlay *mUI;

protected:
  void paintEvent( QPaintEvent *event );

public:
  explicit Overlay( QWidget *parent );
  ~Overlay();

public slots:
  void GameWindowChanged( int x, int y, int w, int h );

};

