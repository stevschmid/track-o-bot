#include "tracker.h"

void Tracker::AddResult(GameMode mode, Outcome outcome, bool coin, Class ownClass, Class opponentClass) {
  std::cout << "GameMode " << mode <<
    " Outcome " << outcome <<
    " Coin " << coin <<
    " OwnCl " << ownClass <<
    " OppCl " << opponentClass << endl;
}
