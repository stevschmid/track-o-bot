#pragma once

#include <QMainWindow>
#include <QVariantMap>
#include <QPainter>
#include <QTimer>
#include <QRect>
#include <QPoint>

namespace Ui { class Overlay; }

#include "../Result.h"

typedef QList< QVariantMap > OverlayHistoryList;

class Overlay : public QMainWindow
{
  Q_OBJECT

private:
  Ui::Overlay *mUI;

  OverlayHistoryList mPlayerHistory;
  OverlayHistoryList mOpponentHistory;

  QMap< QString, QVariantMap > mCardDB;

  Player mShowPlayerHistory;

  QRect mPlayerDeckRect;
  QRect mOpponentDeckRect;

  QTimer mCheckForHoverTimer;

  void LoadCards();
  void UpdateHistoryFor( Player player, const ::CardHistoryList& list );
  void Update();

protected:
  void paintEvent( QPaintEvent *event );

public:
  explicit Overlay( QWidget *parent = 0 );
  ~Overlay();

private slots:
  void CheckForHover();

public slots:
  void HandleGameWindowChanged( int x, int y, int w, int h );
  void HandleGameStarted();
  void HandleGameStopped();

  void HandleCardsDrawnUpdate( const ::CardHistoryList& cardsDrawn );

  void HandleOverlaySettingChanged( bool enabled );

};

