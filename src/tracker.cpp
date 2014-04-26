#include "tracker.h"

void Tracker::AddResult(GameMode mode, Outcome outcome, GoingOrder order, Class ownClass, Class opponentClass) {
  logger << "GameMode " << mode <<
    " Outcome " << outcome <<
    " Order " << order <<
    " OwnCl " << ownClass <<
    " OppCl " << opponentClass << endl;
}
