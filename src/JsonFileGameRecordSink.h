#pragma once

#include "IGameRecordSink.h"
#include "Json.h"
#include "Hearthstone.h"
#include "Tracker.h"

#include <QDesktopServices>
#include <QDir>
#include <QTime>

class JsonFileGameRecordSink : public QObject, public IGameRecordSink
{
  Q_OBJECT
    
DEFINE_SINGLETON( JsonFileGameRecordSink )
    
private:
    QSettings mSettings;
    
protected:
    
    void RecordToJson(QtJson::JsonObject& result, GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass, const CardHistoryList& historyCardList, int durationInSeconds);
    
public:
    
  virtual void AddResult( GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass, const CardHistoryList& cardHistoryList, int durationInSeconds );
    
  void SetExportPath(QString path);
  QString ExportPath();
  
  void SetIsEnabled(bool path);
  bool IsEnabled();
};