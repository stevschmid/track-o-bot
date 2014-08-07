#include "HearthstoneLogTracker.h"

#include <QRegExp>
#include <QStringList>

HearthstoneLogTracker::HearthstoneLogTracker() {
  connect( &mLogWatcher, SIGNAL( LineAdded(QString) ), this, SLOT( HandleLogLine(QString) ) );
  Reset();
}

void HearthstoneLogTracker::Reset() {
  mOutcome = OUTCOME_UNKNOWN;
  mOrder = ORDER_UNKNOWN;
  mCardHistoryList.clear();
}

void HearthstoneLogTracker::HandleLogLine( const QString& line ) {
  if( line.trimmed().isEmpty() )
    return;

  // CardPlayed / CardReturned / PlayerDied
  QRegExp regex( "ProcessChanges.*cardId=(\\w+).*zone from (.*) -> (.*)" );
  if( regex.indexIn(line) != -1 ) {
    QStringList captures = regex.capturedTexts();
    QString cardId = captures[1];
    QString from = captures[2];
    QString to = captures[3];

    bool draw = from.contains( "DECK" ) && to.contains( "HAND" );
    bool mulligan = from.contains( "HAND" ) && to.contains( "DECK" );

    // Discarded cards by playing Soulfire, Doomguard etc.
    bool discard = from.contains( "HAND" ) && to.contains( "GRAVEYARD" );

    if( !draw && !mulligan && !discard ) {
      if( from.contains( "FRIENDLY HAND" ) ) {
        CardPlayed( PLAYER_SELF, cardId.toStdString() );
      } else if( from.contains( "OPPOSING HAND" ) ) {
        CardPlayed( PLAYER_OPPONENT, cardId.toStdString() );
      }
    }

    if( from.contains( "FRIENDLY PLAY" ) && to.contains( "FRIENDLY HAND" ) ) {
      CardReturned( PLAYER_SELF, cardId.toStdString() );
    }

    // Player died? (unfortunately there is no log entry when conceding)
    if( to.contains( "GRAVEYARD" ) && from.contains( "PLAY (Hero)" ) ) {
      if( to.contains( "FRIENDLY" ) ) {
        PlayerDied( PLAYER_SELF );
      } else if( to.contains( "OPPOSING" ) ) {
        PlayerDied( PLAYER_OPPONENT );
      }
    }

#ifdef _DEBUG
    LOG( "Card %s from %s -> %s. (draw: %d, mulligan %d, discard %d)", cardId.toStdString().c_str(), from.toStdString().c_str(), to.toStdString().c_str(), draw, mulligan, discard );
#endif
  }

  // ReceivedCoin
  QRegExp regexCoin( "ProcessChanges.*zonePos=5.*zone from  -> (.*)" );  // unique because from is nothing -> " "
  if( regexCoin.indexIn(line) != -1 ) {
    // The receiving of the coin marks a new game, so reset our state
    Reset();

    QStringList captures = regexCoin.capturedTexts();
    QString to = captures[1];

    if( to.contains( "FRIENDLY HAND" ) ) {
      CoinReceived( PLAYER_SELF );
    } else if( to.contains( "OPPOSING HAND" ) ) {
      CoinReceived( PLAYER_OPPONENT );
    }
  }
}

void HearthstoneLogTracker::PlayerDied( Player player ) { // Not triggered when conceding
  if( player == PLAYER_SELF ) {
    mOutcome = OUTCOME_DEFEAT;
  } else if( player == PLAYER_OPPONENT ) {
    mOutcome = OUTCOME_VICTORY;
  }
}

void HearthstoneLogTracker::CardPlayed( Player player, const string& cardId ) {
  mCardHistoryList.push_back( CardHistoryItem( player, cardId ) );
}

void HearthstoneLogTracker::CardReturned( Player player, const string& cardId ) {
  // Make sure we remove the "Choose One"-cards from the history
  // if we decide to withdraw them after a second of thought
  if( !mCardHistoryList.empty() &&
      mCardHistoryList.back().player == player &&
      mCardHistoryList.back().cardId == cardId )
  {
    mCardHistoryList.pop_back();
  }
}

void HearthstoneLogTracker::CoinReceived( Player player ) {
  if( player == PLAYER_SELF ) {
    // I go second because I get the coin
    mOrder = ORDER_SECOND;
  } else if( player == PLAYER_OPPONENT ) {
    // Opponent got coin, so I go first
    mOrder = ORDER_FIRST;
  }
}
