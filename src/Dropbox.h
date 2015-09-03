#pragma once

#include <QString>

class Dropbox {
private:
  QString mDropboxPath;

  QString RetrieveDropboxPath() const;

public:
  QString AppFolder( const QString& subFolder ) const;
  bool AppFolderExists() const;

  Dropbox();
};
