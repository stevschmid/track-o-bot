#pragma once

#include "ResultTracker.h"

#include <QTimer>
#include <QTime>

#include <vector>

class Core : public QObject
{
  Q_OBJECT

private:
  QTimer*               mTimer;
  bool                  mGameRunning;

  ResultTracker         mResultTracker;

  bool mGameClientRestartRequired;

private slots:
  void Tick();

signals:
  void HandleGameClientRestartRequired( bool restartRequired );

public:
  Core();
  ~Core();
};
