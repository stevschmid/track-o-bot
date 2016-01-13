#pragma once

#include <QString>
#include <QMap>

class Metadata
{
  DEFINE_SINGLETON( Metadata )

private:
  QMap< QString, QString > mMetadata;

public:
  void Add( const QString& key, const QString& value );
  void Add( const QString& key, const char* fmt, ... );
  void Add( const QString& key, int value );

  void Remove( const QString& key );
  void Clear();

  const QMap< QString, QString >& Map() const;
};
