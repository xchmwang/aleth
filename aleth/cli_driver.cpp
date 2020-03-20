
#include "cli_driver.h"
#include "ubre_callback.h"

cli_driver::cli_driver(const std::string &rpc_listen, uint16_t rpc_port)
    : m_rpc_listen(rpc_listen), m_rpc_port(rpc_port) {

  m_thread = std::make_unique<std::thread>([this]() {
    ff::net::net_nervure nn;
    m_p_nn = &nn;
    ff::net::typed_pkg_hub hub;

    hub.to_recv_pkg<nbre_nr_handle_req>([this](
        std::shared_ptr<nbre_nr_handle_req> req) { m_pkgs.push_back(req); });

    hub.to_recv_pkg<nbre_nr_result_by_handle_req>(
        [this](std::shared_ptr<nbre_nr_result_by_handle_req> req) {
          m_pkgs.push_back(req);
        });

    hub.to_recv_pkg<nbre_nr_result_by_height_req>(
        [this](std::shared_ptr<nbre_nr_result_by_height_req> req) {
          m_pkgs.push_back(req);
        });

    hub.to_recv_pkg<nbre_nr_sum_req>([this](
        std::shared_ptr<nbre_nr_sum_req> req) { m_pkgs.push_back(req); });
    hub.to_recv_pkg<nbre_dip_reward_req>([this](
        std::shared_ptr<nbre_dip_reward_req> req) { m_pkgs.push_back(req); });

    hub.to_recv_pkg<nbre_experiment_req>([this](
        std::shared_ptr<nbre_experiment_req> req) { m_pkgs.push_back(req); });
    hub.to_recv_pkg<nbre_lib_req>(
        [this](std::shared_ptr<nbre_lib_req> req) { m_pkgs.push_back(req); });

    nn.get_event_handler()
        ->listen<::ff::net::event::more::tcp_server_accept_connection>(
            [this](::ff::net::tcp_connection_base_ptr conn) { m_conn = conn; });

    nn.add_pkg_hub(hub);
    nn.add_tcp_server(m_rpc_listen, m_rpc_port);
    nn.run();
  });
}

cli_driver::~cli_driver() {
  if (m_p_nn) {
    m_p_nn->stop();
  }
  if (m_thread) {
    m_thread->join();
  }
}

void cli_driver::handle_cli_pkgs() {
  while (!m_pkgs.empty()) {
    auto ret = m_pkgs.try_pop_front();
    if (!ret.first) {
      continue;
    }
    auto pkg = ret.second;
    if (pkg->type_id() == nbre_nr_handle_req_pkg) {
      nbre_nr_handle_req *req = (nbre_nr_handle_req *)pkg.get();

      callback_handler::instance().add_nr_handler(
          req->get<p_holder>(),
          [this](uint64_t holder, const char *nr_handle_id) {
            auto ack = std::make_shared<nbre_nr_handle_ack>();
            ack->set<p_holder>(holder);
            ack->set<p_nr_handle>(std::string(nr_handle_id));
            m_conn->send(ack);
          });
      LOG(INFO) << "forward nr handle req";
      ipc_nbre_nr_handle(reinterpret_cast<void *>(req->get<p_holder>()),
                         req->get<p_start_block>(), req->get<p_end_block>(),
                         req->get<p_nr_version>());

    } else if (pkg->type_id() == nbre_nr_result_by_handle_req_pkg) {
      nbre_nr_result_by_handle_req *req =
          (nbre_nr_result_by_handle_req *)pkg.get();
      callback_handler::instance().add_nr_result_handler(
          req->get<p_holder>(), [this](uint64_t holder, const char *nr_result) {
            auto ack = std::make_shared<nbre_nr_result_by_handle_ack>();
            ack->set<p_holder>(holder);
            ack->set<p_nr_result>(std::string(nr_result));
            m_conn->send(ack);
          });
      ipc_nbre_nr_result_by_handle(
          reinterpret_cast<void *>(req->get<p_holder>()),
          req->get<p_nr_handle>().c_str());
    } else if (pkg->type_id() == nbre_nr_result_by_height_req_pkg) {
      LOG(INFO) << "handle pkg nr result by height req";
      nbre_nr_result_by_height_req *req =
          (nbre_nr_result_by_height_req *)pkg.get();
      callback_handler::instance().add_nr_result_by_height_handler(
          req->get<p_holder>(), [this](uint64_t holder, const char *nr_result) {
            auto ack = std::make_shared<nbre_nr_result_by_height_ack>();
            ack->set<p_holder>(holder);
            ack->set<p_nr_result>(std::string(nr_result));
            m_conn->send(ack);
          });
      ipc_nbre_nr_result_by_height(
          reinterpret_cast<void *>(req->get<p_holder>()), req->get<p_height>());
    } else if (pkg->type_id() == nbre_nr_sum_req_pkg) {
      nbre_nr_sum_req *req = (nbre_nr_sum_req *)pkg.get();
      callback_handler::instance().add_nr_sum_handler(
          req->get<p_holder>(), [this](uint64_t holder, const char *nr_sum) {
            auto ack = std::make_shared<nbre_nr_sum_ack>();
            ack->set<p_holder>(holder);
            ack->set<p_nr_sum>(std::string(nr_sum));
            m_conn->send(ack);
          });
      ipc_nbre_nr_sum(reinterpret_cast<void *>(req->get<p_holder>()),
                      req->get<p_height>());
    } else if (pkg->type_id() == nbre_dip_reward_req_pkg) {
      nbre_dip_reward_req *req = (nbre_dip_reward_req *)pkg.get();
      callback_handler::instance().add_dip_reward_handler(
          req->get<p_holder>(),
          [this](uint64_t holder, const char *dip_reward) {
            auto ack = std::make_shared<nbre_dip_reward_ack>();
            ack->set<p_holder>(holder);
            ack->set<p_dip_reward>(std::string(dip_reward));
            m_conn->send(ack);
          });
      ipc_nbre_dip_reward(reinterpret_cast<void *>(req->get<p_holder>()),
                          req->get<p_height>(), req->get<p_version>());
    } else if (pkg->type_id() == nbre_experiment_req_pkg) {
      nbre_experiment_req *req = (nbre_experiment_req *)pkg.get();
      callback_handler::instance().add_experiment_handler(
          req->get<p_holder>(), [this](uint64_t holder, const char *ret) {
            LOG(INFO) << "dummy neb recv nbre experiment ack";
            auto ack = std::make_shared<nbre_experiment_ack>();
            ack->set<p_holder>(holder);
            ack->set<p_msg>(std::string(ret));
            m_conn->send(ack);
          });
      ipc_nbre_experiment(reinterpret_cast<void *>(req->get<p_holder>()),
                          req->get<p_version>(), req->get<p_msg>().c_str());
    } else if (pkg->type_id() == nbre_lib_req_pkg) {
      nbre_lib_req *req = (nbre_lib_req *)pkg.get();
      callback_handler::instance().add_lib_handler(
          req->get<p_holder>(), [this](uint64_t holder, int32_t ret) {
            LOG(INFO) << "dummy neb recv nbre lib ack";
            auto ack = std::make_shared<nbre_lib_ack>();
            ack->set<p_holder>(holder);
            ack->set<p_status>(ret);
            m_conn->send(ack);
          });
      ipc_nbre_lib(reinterpret_cast<void *>(req->get<p_holder>()),
                   req->get<p_version>(), req->get<p_msg>().c_str());
    } else {
      LOG(INFO) << "pkg type id " << pkg->type_id() << " not found";
    }
  }
}
