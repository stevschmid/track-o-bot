#pragma once

class CocoaInitializer
{
public:
  CocoaInitializer();
  ~CocoaInitializer();

private:
  class Private;
  Private* d;
};

