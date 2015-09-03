#include "ReplayRecorder.h"
#include "Hearthstone.h"
#include "Dropbox.h"
#include "Settings.h"

#include <QDir>
#include <QTimer>
#include <QPixmap>
#include <QTemporaryFile>

#define REPLAY_WIDTH   1280
#define REPLAY_HEIGHT  800
#define REPLAY_BITRATE 500
#define REPLAY_FPS     1

ReplayRecorder::ReplayRecorder( HearthstoneLogTracker *logTracker )
  : mSpectating( false ),
    mWriter(
      REPLAY_WIDTH, REPLAY_HEIGHT,
      REPLAY_BITRATE, REPLAY_FPS
    )
{
  connect( logTracker, SIGNAL( HandleMatchStart() ), this, SLOT( HandleMatchStart() ) );
  connect( logTracker, SIGNAL( HandleMatchEnd(const ::CardHistoryList&, bool) ), this, SLOT( HandleMatchEnd() ) );

  connect( logTracker, SIGNAL( HandleTurn(int) ), this, SLOT( HandleTurn(int) ) );
  connect( logTracker, SIGNAL( HandleSpectating(bool) ), this, SLOT( HandleSpectating(bool) ) );
}

ReplayRecorder::~ReplayRecorder() {
  StopRecording();
}

bool ReplayRecorder::CanRecord() const {
  return Settings::Instance()->ReplayRequirementsFulfilled() &&
    Settings::Instance()->ReplaysEnabled();
}

void ReplayRecorder::HandleSpectating( bool nowSpectating ) {
  mSpectating = nowSpectating;
}

void ReplayRecorder::StartRecording() {
  StopRecording();

  if( CanRecord() ) {
    if( mWriter.IsOpen() ) {
      mWriter.Close();
    }

    // Create a unique path using tmpFile
    QTemporaryFile tmpFile;
    if( tmpFile.open() ) {
      tmpFile.setAutoRemove( false );
      mTempReplayPath = tmpFile.fileName();
      tmpFile.close();

      if( !mWriter.Open( mTempReplayPath ) ) {
        ERR( "Couldn't open replay writer %s\n", qt2cstr( mTempReplayPath ) );
      } else {
        DBG( "Opened temp. replay file %s", qt2cstr( mTempReplayPath ) );
      }
    } else {
      ERR( "Couldn't create a temp. replay file" );
    }
  }
}

void ReplayRecorder::StopRecording() {
  if( mWriter.IsOpen() ) {
    mWriter.Close();
  }
}

bool ReplayRecorder::IsRecording() const {
  return mWriter.IsOpen();
}

void ReplayRecorder::RecordScreen() {
  QPixmap screen;

  if( Hearthstone::Instance()->CaptureWholeScreen( &screen ) ) {
    mWriter.AddFrame( screen.toImage() );
  } else {
    ERR( "Couldn't capture screen" );
  }
}

void ReplayRecorder::HandleMatchStart() {
  StartRecording();
}

void ReplayRecorder::HandleMatchEnd() {
  StopRecording();
}

void ReplayRecorder::HandleTurn( int turnCounter ) {
  UNUSED_ARG( turnCounter );

  if( IsRecording() ) {
    QTimer::singleShot( 3500, this, [=]() {
      RecordScreen();
    });
  }
}

void ReplayRecorder::SaveReplay( int resultId ) {
  StopRecording();

  if( mTempReplayPath.isEmpty() || !QFile::exists( mTempReplayPath )  ) {
    // In case we did not record anything
    return;
  }

  QString src = mTempReplayPath;
  QString dst = Dropbox().AppFolder( QString( "%1.webm" ).arg( resultId ) );
  if( !QFile::rename( src, dst ) ) {
    ERR( "Couldn't move replay" );
  } else {
    DBG( "Replay saved to %s", qt2cstr( dst ) );
  }

  mTempReplayPath = "";
}

