
#pragma once
#include "common.h"
#include <ff/network.h>

class cli_driver {
public:
  cli_driver(const std::string &rpc_listen, uint16_t rpc_port);
  virtual ~cli_driver();
  void handle_cli_pkgs();

protected:
  std::string m_rpc_listen;
  uint16_t m_rpc_port;

  std::unique_ptr<std::thread> m_thread;
  neb::util::wakeable_queue<std::shared_ptr<ff::net::package>> m_pkgs;
  ff::net::tcp_connection_base_ptr m_conn;
  ff::net::net_nervure *m_p_nn;
};
