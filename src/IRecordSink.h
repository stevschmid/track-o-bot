#pragma once

struct IRecordSink
{
    virtual void AddResult( GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass, const CardHistoryList& cardHistoryList, int durationInSeconds ) = 0;
  
  virtual QString SinkId() = 0;
    
};