#pragma once

class Tracker
{
  DEFINE_SINGLETON(Tracker);

public:
  void AddResult(GameMode mode, Outcome outcome, bool coin, Class ownClass, Class opponentClass) {
    std::cout << "GameMode " << mode <<
      " Outcome " << outcome <<
      " Coin " << coin <<
      " OwnCl " << ownClass <<
      " OppCl " << opponentClass << endl;
  }
};

