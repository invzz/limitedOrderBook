#pragma once
#include <string>

#define GET_METRICS "GET_METRICS"
#define PUT_ORDER   "PUT_ORDER"
#define STOP        "STOP"

class Command
{
  public:
  virtual ~Command()                            = default;
  virtual void execute(const std::string &body) = 0;
};