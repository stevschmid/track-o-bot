#pragma once

struct Result;

struct IResultSink
{
  virtual void AddResult( Result ) = 0;
  virtual QString SinkId() = 0;
  virtual bool IsEnabled() = 0;
};