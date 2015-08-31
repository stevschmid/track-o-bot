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
  QString mDropboxPath;
  WebMWriter mWriter;

  QString AppFile( const QString& subFolder ) const;
  static QString RetrieveDropboxPath();

private slots:
  void HandleMatchStart();

  void HandleTurn( int turnCounter );
  void HandleSpectating( bool nowSpectating );

  void UploadResultSucceeded( const QJsonObject& response );

public:
  ReplayManager( HearthstoneLogTracker *logTracker );
  ~ReplayManager();
};
