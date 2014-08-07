#include "HearthstoneLogAnalyzer.h"

#include <QRegExp>
#include <QStringList>

HearthstoneLogAnalyzer::HearthstoneLogAnalyzer() {
  connect( &logWatcher, SIGNAL( LineAdded(QString) ), this, SLOT( HandleLogLine(QString) ) );
}

void HearthstoneLogAnalyzer::HandleLogLine( const QString& line ) {
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
        emit CardPlayed( PLAYER_SELF, cardId.toStdString() );
      } else if( from.contains( "OPPOSING HAND" ) ) {
        emit CardPlayed( PLAYER_OPPONENT, cardId.toStdString() );
      }
    }

    if( from.contains( "FRIENDLY PLAY" ) && to.contains( "FRIENDLY HAND" ) ) {
      emit CardReturned( PLAYER_SELF, cardId.toStdString() );
    }

    // Player died? (unfortunately there is no log entry when conceding)
    if( to.contains( "GRAVEYARD" ) && from.contains( "PLAY (Hero)" ) ) {
      if( to.contains( "FRIENDLY" ) ) {
        emit PlayerDied( PLAYER_SELF );
      } else if( to.contains( "OPPOSING" ) ) {
        emit PlayerDied( PLAYER_OPPONENT );
      }
    }

#ifdef _DEBUG
    LOG( "Card %s from %s -> %s. (draw: %d, mulligan %d, discard %d)", cardId.toStdString().c_str(), from.toStdString().c_str(), to.toStdString().c_str(), draw, mulligan, discard );
#endif
  }

  // ReceivedCoin
  QRegExp regexCoin( "ProcessChanges.*zonePos=5.*zone from  -> (.*)" );  // unique because from is nothing -> " "
  if( regexCoin.indexIn(line) != -1 ) {
    QStringList captures = regexCoin.capturedTexts();
    QString to = captures[1];

    if( to.contains( "FRIENDLY HAND" ) ) {
      emit CoinReceived( PLAYER_SELF );
    } else if( to.contains( "OPPOSING HAND" ) ) {
      emit CoinReceived( PLAYER_OPPONENT );
    }
  }
}
