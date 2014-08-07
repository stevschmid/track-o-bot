#include "Hearthstone.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <QApplication>

#include "MockWindowCapture.h"

using ::testing::Return;

class HearthstoneTest : public ::testing::Test {
public:
  QApplication *mApp;
  MockWindowCapture *mWindowCapture;

  virtual void SetUp() {
    mWindowCapture = new MockWindowCapture;
    Hearthstone::Instance()->SetWindowCapture( mWindowCapture );

    int argc = 0;
    mApp = new QApplication( argc, NULL );
  }

  virtual void TearDown() {
    delete mApp;
  }
};

TEST_F(HearthstoneTest, IsRunningWhenWindowFound) {
  EXPECT_CALL( *mWindowCapture, WindowFound() )
    .WillOnce( Return( true ) );
  EXPECT_EQ( Hearthstone::Instance()->Running(), true );
}

TEST_F(HearthstoneTest, IsRunningWhenWindowNotFound) {
  EXPECT_CALL( *mWindowCapture, WindowFound() )
    .WillOnce( Return( false ) );
  EXPECT_EQ( Hearthstone::Instance()->Running(), false );
}

TEST_F(HearthstoneTest, CaptureSameAspectRatio) {
  // Ingame is 1280x1024, but our virtual canvas is fixed to 1024x768
  EXPECT_CALL( *mWindowCapture, Width() )
    .WillOnce( Return( 1280 ) );
  EXPECT_CALL( *mWindowCapture, Height() )
    .WillOnce( Return( 1024 ) );

  EXPECT_CALL( *mWindowCapture, Capture( 640, 512, 43, 43 ) )
    .WillOnce( Return( QPixmap( 1, 1 ) ) );

  Hearthstone::Instance()->Capture( 512, 384, 32, 32 );
}

TEST_F(HearthstoneTest, CaptureAspectRatioIsDifferent) {
  EXPECT_CALL( *mWindowCapture, Width() )
    .WillOnce( Return( 1280 ) );
  EXPECT_CALL( *mWindowCapture, Height() )
    .WillOnce( Return( 800 ) );

  // Rounding is important
  // x = 888.96, y = 368.75
  EXPECT_CALL( *mWindowCapture, Capture( 889, 369, 57, 57 ) )
    .WillOnce( Return(QPixmap( 1, 1 ) ) );

  Hearthstone::Instance()->Capture(751, 354, 55, 55);
}

