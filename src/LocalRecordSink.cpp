#include "LocalRecordSink.h"

#define DEFAULT_EXPORT_DIR QDir::homePath()
const bool DEFAULT_IS_ENABLED = false;

DEFINE_SINGLETON_SCOPE( LocalRecordSink );

LocalRecordSink::LocalRecordSink() {
}

LocalRecordSink::~LocalRecordSink() {
}

void LocalRecordSink::AddResult( GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass, const CardHistoryList& cardHistoryList, int durationInSeconds )
{
    // write json to a text file in the export directory
    QtJson::JsonObject gamejson;
    this->RecordToJson(gamejson, mode, outcome, order, ownClass, opponentClass, cardHistoryList, durationInSeconds);
    
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

void LocalRecordSink::RecordToJson(QtJson::JsonObject& result, GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass, const CardHistoryList& historyCardList, int durationInSeconds)
{
  if (!IsEnabled()) return;
  
  result[ "coin" ]     = ( order == ORDER_SECOND );
  result[ "hero" ]     = CLASS_NAMES[ ownClass ];
  result[ "opponent" ] = CLASS_NAMES[ opponentClass ];
  result[ "win" ]      = ( outcome == OUTCOME_VICTORY );
  result[ "mode" ]     = MODE_NAMES[ mode ];
  result[ "duration" ] = durationInSeconds;
  
  QtJson::JsonArray card_history;
  for( CardHistoryList::const_iterator it = historyCardList.begin(); it != historyCardList.end(); ++it ) {
    QtJson::JsonObject item;
    item[ "turn" ] = (*it).turn;
    item[ "player" ] = (*it).player == PLAYER_SELF ? "me" : "opponent";
    item[ "card_id" ] = (*it).cardId.c_str();
    card_history.append(item);
  }
  result[ "card_history" ] = card_history;
}

void LocalRecordSink::SetExportPath(QString path)
{
    mSettings.setValue( "jsonfilepath", path );
}

QString LocalRecordSink::ExportPath()
{
    if( !mSettings.contains( "jsonfilepath" ) || mSettings.value( "jsonfilepath" ).toString().isEmpty() ) {
        SetExportPath( DEFAULT_EXPORT_DIR );
    }
    
    return mSettings.value( "jsonfilepath" ).toString();
}

bool LocalRecordSink::IsEnabled()
{
  if( !mSettings.contains( "jsonfileEnabled" ) || mSettings.value( "jsonfileEnabled" ).toString().isEmpty() ) {
    SetIsEnabled( DEFAULT_IS_ENABLED );
  }
  
  return mSettings.value( "jsonfileEnabled" ).toBool();
}

void LocalRecordSink::SetIsEnabled(bool enabled)
{
  return mSettings.setValue( "jsonfileEnabled", enabled );
}

