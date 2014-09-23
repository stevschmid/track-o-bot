#pragma once

#include "IResultSink.h"
#include "Json.h"
#include "Hearthstone.h"
#include "Tracker.h"
#include "Result.h"

#include <QDesktopServices>
#include <QDir>
#include <QTime>

class LocalResultSink : public QObject, public IResultSink
{
  Q_OBJECT
    
DEFINE_SINGLETON( LocalResultSink )
    
private:
    QSettings mSettings;

public:
    
  void AddResult( Result game );
  
  QString SinkId() { return "JsonFile"; }

  void SetExportPath(QString path);
  QString ExportPath();
  
  void SetIsEnabled(bool path);
  bool IsEnabled();
};