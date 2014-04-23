#include "window_capture.h"

class MockWindowCapture : public WindowCapture
{
public:
  MOCK_METHOD0(WindowFound, bool());
  MOCK_METHOD0(GetWidth, int());
  MOCK_METHOD0(GetHeight, int());
  MOCK_METHOD4(Capture, QPixmap(int x, int y, int w, int h));
};
