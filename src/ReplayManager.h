#pragma once

#include "HearthstoneLogTracker.h"
#include "Tracker.h"
#include "WebMWriter.h"

#include <QObject>
#include <QMap>
#include <QPixmap>

class ReplayManager : public QObject
{
  Q_OBJECT

private:
  bool mSpectating;
  WebMWriter mWriter;

  static QString AppFolder( const QString& subFolder );
  static QString AppFolder();
  static QString RetrieveDropboxPath();

private slots:
  void HandleMatchStart();

  void HandleTurn( int turnCounter );
  void HandleSpectating( bool nowSpectating );

  void UploadResultSucceeded( const QJsonObject& response );

public:
  ReplayManager( HearthstoneLogTracker *logTracker );
  ~ReplayManager();

  bool IsEnabled() const;

  static bool CanRecordReplays();
};
