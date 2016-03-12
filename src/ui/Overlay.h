#pragma once

#include <QMainWindow>
#include <QVariantMap>

namespace Ui { class Overlay; }

#include "../Result.h"

class Overlay : public QMainWindow
{
  Q_OBJECT

private:
  Ui::Overlay *mUI;

  QList< QVariantMap > mPlayerHistory;
  QList< QVariantMap > mOpponentHistory;

  QMap< QString, QVariantMap > mCardDB;

  void LoadCards();

protected:
  void paintEvent( QPaintEvent *event );

public:
  explicit Overlay( QWidget *parent = 0 );
  ~Overlay();

public slots:
  void HandleGameWindowChanged( int x, int y, int w, int h );
  void HandleGameStarted();
  void HandleGameStopped();

  void HandleCardHistoryListUpdate( const ::CardHistoryList& cardHistoryList );

};

