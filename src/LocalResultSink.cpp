#include "LocalResultSink.h"

#define DEFAULT_EXPORT_DIR QDir::homePath()
const bool DEFAULT_IS_ENABLED = false;

DEFINE_SINGLETON_SCOPE( LocalResultSink );

LocalResultSink::LocalResultSink() {
}

LocalResultSink::~LocalResultSink() {
}

void LocalResultSink::AddResult( Result game )
{
  QtJson::JsonObject gamejson = game.AsJson();
  QString jsonText = QtJson::serializeStr( gamejson );
  
  QString pathString = ExportPath() +
  "/game." +
  QTime::currentTime().toString("hh.mm.ss.") +
  Tracker::Instance()->Username() +
  ".json";
  
  QFile file( pathString  );
  file.open( QIODevice::WriteOnly | QIODevice::Text );
  QTextStream out( &file );
  out << jsonText;
  file.close();
  
  LOG( "Match data written to " + pathString.toAscii() );
}

void LocalResultSink::SetExportPath(QString path)
{
    mSettings.setValue( "jsonfilepath", path );
}

QString LocalResultSink::ExportPath()
{
    if( !mSettings.contains( "jsonfilepath" ) || mSettings.value( "jsonfilepath" ).toString().isEmpty() ) {
        SetExportPath( DEFAULT_EXPORT_DIR );
    }
    
    return mSettings.value( "jsonfilepath" ).toString();
}

bool LocalResultSink::IsEnabled()
{
  if( !mSettings.contains( "jsonfileEnabled" ) || mSettings.value( "jsonfileEnabled" ).toString().isEmpty() ) {
    SetIsEnabled( DEFAULT_IS_ENABLED );
  }
  
  return mSettings.value( "jsonfileEnabled" ).toBool();
}

void LocalResultSink::SetIsEnabled(bool enabled)
{
  return mSettings.setValue( "jsonfileEnabled", enabled );
}

