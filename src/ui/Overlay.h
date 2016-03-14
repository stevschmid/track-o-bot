#pragma once

#include <QMainWindow>
#include <QVariantMap>
#include <QPainter>
#include <QTimer>
#include <QRect>

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

  Player mShowPlayerHistory;
  QRect mPlayerDeckRect;
  QRect mOpponentDeckRect;

  QTimer mCheckForHoverTimer;

  void LoadCards();
  void PaintHistory( QPainter& painter, int x, int y, int width, const QString& title, QList< QVariantMap >& history );
  void UpdateHistoryFor( Player player, const ::CardHistoryList& list );

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

  void HandleCardHistoryListUpdate( const ::CardHistoryList& cardHistoryList );
  void HandleCardDrawHistoryListUpdate( const ::CardHistoryList& cardDrawHistoryList );

};

