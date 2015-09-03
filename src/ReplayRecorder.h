#pragma once

#include "HearthstoneLogTracker.h"
#include "WebMWriter.h"

#include <QObject>
#include <QString>

class ReplayRecorder : public QObject
{
  Q_OBJECT

private:
  bool mSpectating;
  WebMWriter mWriter;
  QString mTempReplayPath;

  void StartRecording();
  void StopRecording();
  void RecordScreen();

  bool IsRecording() const;

private slots:
  void HandleMatchStart();
  void HandleMatchEnd();

  void HandleTurn( int turnCounter );
  void HandleSpectating( bool nowSpectating );

public slots:
  void SaveReplay( int replayId );

public:
  ReplayRecorder( HearthstoneLogTracker *logTracker );
  ~ReplayRecorder();

  bool CanRecord() const;
};
