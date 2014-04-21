typedef quint64 dhash;

//
// C++ implementation of Dhash as outlined here: http://www.hackerfactor.com/blog/?/archives/529-Kind-of-Like-That.html
//

class DHash
{
public:
  static unsigned int CalculateHammingDistance(dhash v1, dhash v2);
  dhash ForPixmap(const QPixmap& pixmap);
};
