#pragma once

#include <QMap>
#include <QVariant>
#include <QRegularExpression>

class HearthstoneLogLineHandler : public QObject {
  Q_OBJECT

private:
  QString mModule;
  QRegularExpression mRegex;

  // Convert "[a=1 b=2]" to map
  QVariant ExtractValue( const QString& str ) {
    QVariant ret;

    if( !( str.startsWith( "[" ) && str.endsWith( "]" ) ) ) {
      return str;
    }

    // extract key=value pairs
    QVariantMap map;

    QRegularExpression r( "[\\S]+=(?:\\s?(?!\\S+=)\\S*)+" );
    QRegularExpressionMatchIterator it = r.globalMatch( str );
    while( it.hasNext() ) {
      QString keyValue = it.next().captured();
      QStringList split = keyValue.split( "=" );
      map[ split.front() ] = split.back();
    }

    return map;
  }

signals:
  void Handle( const QVariantMap& args );

public:
  HearthstoneLogLineHandler( QObject *parent, const QString& module, const QString& regex )
    : QObject( parent ), mModule( module ), mRegex( regex )
  {
  }

  bool Process( const QString& line ) {
    // Check if line is eligible
    if( !mModule.isEmpty() && !line.contains( mModule ) )  {
      return false;
    }

    QRegularExpressionMatch match = mRegex.match( line );
    if( !mRegex.pattern().isEmpty() && !match.hasMatch() ) {
      return false;
    }

    // Extract args
    QVariantMap args;
    for( int i = 1; i <= match.lastCapturedIndex(); i++ ) {
      QString name = mRegex.namedCaptureGroups()[ i ];
      QVariant value = ExtractValue( match.captured( i ) );
      args.insert( name, value );
    }

    emit Handle( args );
    return true;
  }
};

