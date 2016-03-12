#pragma once

#include <QMainWindow>

namespace Ui { class Overlay; }

#include "../Result.h"

class Overlay : public QMainWindow
{
  Q_OBJECT

private:
  Ui::Overlay *mUI;
  CardHistoryList mCardHistoryList;

protected:
  void paintEvent( QPaintEvent *event );

public:
  explicit Overlay( QWidget *parent = 0 );
  ~Overlay();

public slots:
  void GameWindowChanged( int x, int y, int w, int h );
  void HandleCardHistoryListUpdate( const ::CardHistoryList& cardHistoryList );

};

