#pragma once

#include "IRecordSink.h"
#include "Json.h"
#include "Hearthstone.h"
#include "Tracker.h"

#include <QDesktopServices>
#include <QDir>
#include <QTime>

class LocalRecordSink : public QObject, public IRecordSink
{
  Q_OBJECT
    
DEFINE_SINGLETON( LocalRecordSink )
    
private:
    QSettings mSettings;
    
protected:
    
    void RecordToJson(QtJson::JsonObject& result, GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass, const CardHistoryList& historyCardList, int durationInSeconds);
    
public:
    
  void AddResult( GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass, const CardHistoryList& cardHistoryList, int durationInSeconds );
  
  QString SinkId() { return "JsonFile"; }

  void SetExportPath(QString path);
  QString ExportPath();
  
  void SetIsEnabled(bool path);
  bool IsEnabled();
};