#include "hearthstone.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <QApplication>

#include "mock_window_capture.h"

using ::testing::Return;

class HearthstoneTest : public ::testing::Test {
private:
  virtual void SetUp() {
    window_capture = new MockWindowCapture;
    Hearthstone::Instance()->SetWindowCapture(window_capture);

    int argc = 0;
    app = new QApplication(argc, NULL);
  }

  virtual void TearDown() {
    delete app;
  }

  QApplication *app;
  MockWindowCapture *window_capture;
};

TEST_F(HearthstoneTest, IsRunningWhenWindowFound) {
  EXPECT_CALL(*window_capture, WindowFound())
    .WillOnce(Return(true));
  EXPECT_EQ(Hearthstone::Instance()->IsRunning(), true);
}

TEST_F(HearthstoneTest, IsRunningWhenWindowNotFound) {
  EXPECT_CALL(*window_capture, WindowFound())
    .WillOnce(Return(false));
  EXPECT_EQ(Hearthstone::Instance()->IsRunning(), false);
}

TEST_F(HearthstoneTest, CaptureSameAspectRatio) {
  // Ingame is 1280x1024, but our virtual canvas is fixed to 1024x768
  EXPECT_CALL(*window_capture, GetWidth())
    .WillOnce(Return(1280));
  EXPECT_CALL(*window_capture, GetHeight())
    .WillOnce(Return(1024));

  EXPECT_CALL(*window_capture, Capture(640, 512, 43, 43))
    .WillOnce(Return(QPixmap(1, 1)));

  Hearthstone::Instance()->Capture(512, 384, 32, 32);
}

TEST_F(HearthstoneTest, CaptureAspectRatioIsDifferent) {
  EXPECT_CALL(*window_capture, GetWidth())
    .WillOnce(Return(1280));
  EXPECT_CALL(*window_capture, GetHeight())
    .WillOnce(Return(800));

  // Rounding is important
  // x = 888.96, y = 368.75
  EXPECT_CALL(*window_capture, Capture(889, 369, 57, 57))
    .WillOnce(Return(QPixmap(1, 1)));

  Hearthstone::Instance()->Capture(751, 354, 55, 55);
}

