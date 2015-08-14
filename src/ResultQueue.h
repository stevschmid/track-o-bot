#pragma once

#include "Result.h"

class ResultQueue : public QObject
{
  Q_OBJECT

private slots:
  void UploadResultFailed( const Result& result );

public:
  ResultQueue();
  ~ResultQueue();

  void Add( const Result& result );
};
