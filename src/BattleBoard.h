#pragma once

#include "HearthstoneLogTracker.h"
#include "Tracker.h"

#include <QObject>
#include <QMap>
#include <QPixmap>

class BattleBoard : public QObject
{
  Q_OBJECT

private:
  bool mSpectating;

  QMap< QString, QPixmap > mScreenshots;

private slots:
  void HandleMatchStart();

  void HandleTurn( int turnCounter );
  void HandleSpectating( bool nowSpectating );

  void UploadResultSucceeded( const QJsonObject& response );

public:
  BattleBoard( HearthstoneLogTracker *logTracker );

};
