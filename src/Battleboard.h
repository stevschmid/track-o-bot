#pragma once

#include "HearthstoneLogTracker.h"
#include "Tracker.h"

#include <QObject>
#include <QMap>
#include <QPixmap>

class Battleboard : public QObject
{
  Q_OBJECT

private:
  bool mSpectating;
  QString mDropboxPath;

  void ClearScreenshots();
  bool MoveScreenshots( const QString& destination );

  bool EnsureDirectory( const QString& path );
  QString AppDirectory( const QString& subFolder ) const;
  void SaveScreenshot( const QString& name, const QPixmap& screenshot );

  static QString RetrieveDropboxPath();

private slots:
  void HandleMatchStart();

  void HandleTurn( int turnCounter );
  void HandleSpectating( bool nowSpectating );

  void UploadResultSucceeded( const QJsonObject& response );

public:
  Battleboard( HearthstoneLogTracker *logTracker );
};
