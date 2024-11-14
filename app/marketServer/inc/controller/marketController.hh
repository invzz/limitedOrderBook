
#pragma once
#include "command.hh"
#include "marketServer.hh"

class GetMetricsCommand : public Command
{
  public:
  GetMetricsCommand(MarketServer *server, const std::string &userId) : server_(server), userId_(userId) {}

  void execute(const std::string & /* body */) override { server_->GetMetrics(userId_); }

  private:
  MarketServer *server_;
  std::string   userId_;
};

class PutOrderCommand : public Command
{
  public:
  PutOrderCommand(MarketServer *server) : server_(server) {}

  void execute(const std::string &body) override { server_->PutOrder(body); }

  private:
  MarketServer *server_;
};

class StopCommand : public Command
{
  public:
  StopCommand(MarketServer *server) : server_(server) {}

  void execute(const std::string & /* body */) override { server_->stop(); }

  private:
  MarketServer *server_;
};