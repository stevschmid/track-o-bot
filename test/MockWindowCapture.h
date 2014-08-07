#include "WindowCapture.h"

class MockWindowCapture : public WindowCapture
{
public:
  MOCK_METHOD0( WindowFound, bool() );
  MOCK_METHOD0( Width, int() );
  MOCK_METHOD0( Height, int() );
  MOCK_METHOD4( Capture, QPixmap( int x, int y, int w, int h ) );
};
