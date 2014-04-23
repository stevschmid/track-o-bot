#include "hearthstone.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <QApplication>

#include "mock_window_capture.h"

using ::testing::Return;

class HearthstoneTest : public ::testing::Test {
protected:
  virtual void SetUp() {
    Hearthstone::Instance()->SetWindowCapture(&window_capture);

    int argc = 0;
    app = new QApplication(argc, NULL);
  }

  virtual void TearDown() {
    delete app;
  }

  QApplication *app;

  MockWindowCapture window_capture;
};

TEST_F(HearthstoneTest, IsRunningWhenWindowFound) {
  EXPECT_CALL(window_capture, WindowFound())
    .WillOnce(Return(true));
  EXPECT_EQ(Hearthstone::Instance()->IsRunning(), true);
}

TEST_F(HearthstoneTest, IsRunningWhenWindowNotFound) {
  EXPECT_CALL(window_capture, WindowFound())
    .WillOnce(Return(false));
  EXPECT_EQ(Hearthstone::Instance()->IsRunning(), false);
}

TEST_F(HearthstoneTest, CaptureTrivial) {
  // Ingame is 1280x800, but our virtual canvas is fixed to 1024x768
  EXPECT_CALL(window_capture, GetWidth())
    .WillOnce(Return(1280));
  EXPECT_CALL(window_capture, GetHeight())
    .WillOnce(Return(800));

  EXPECT_CALL(window_capture, Capture(640, 400, 33, 33))
    .WillOnce(Return(QPixmap(1, 1)));

  Hearthstone::Instance()->Capture(512, 384, 32, 32);
}

