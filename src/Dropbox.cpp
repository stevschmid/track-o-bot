#include "Dropbox.h"

#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>

Dropbox::Dropbox()
  : mDropboxPath( RetrieveDropboxPath() )
{
}

QString Dropbox::RetrieveDropboxPath() const {
#ifdef Q_OS_MAC
  QString path = QDir::homePath() + "/.dropbox/host.db";
#else
  QString path = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ).first() + "/Dropbox/host.db";
#endif

  QFile file( path );
  if( !file.open( QFile::ReadOnly | QFile::Text ) )
    return QString();

  QString hostDbContents;

  QTextStream in( &file );
  while( !in.atEnd() ) {
    QString line = in.readLine();
    if( !line.isEmpty() ) {
      hostDbContents = line;
    }
  }

  if( hostDbContents.isEmpty() )
    return QString();

  QByteArray bytes;
  bytes.append( hostDbContents );
  return QByteArray::fromBase64( bytes );
}

QString Dropbox::AppFolder( const QString& fileName ) const {
  if( mDropboxPath.isEmpty() )
    return QString();

  QString path = mDropboxPath + "/Apps/Track-o-Bot/" + fileName;
  return path;
}

bool Dropbox::AppFolderExists() const {
  return QDir( AppFolder( "" ) ).exists();
}

