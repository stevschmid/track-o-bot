//
//  IGameRecordSink.h
//

#pragma once

struct IGameRecordSink
{
    virtual void AddResult( GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass, const CardHistoryList& cardHistoryList, int durationInSeconds ) = 0;
    
};