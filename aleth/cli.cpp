
#include "common.h"
#include <boost/process.hpp>
#include <boost/program_options.hpp>
#include <ff/network.h>
#include <libubre/fs/util.h>

namespace po = boost::program_options;
namespace bp = boost::process;

po::variables_map get_variables_map(int argc, char *argv[]) {
  po::options_description desc("Query CLI tool");
  // clang-format off
  desc.add_options()("help", "show help message")
    ("query", po::value<std::string>(), "nr, nr-result, nr-sum, dip-reward, exp, lib")
    ("start-block", po::value<uint64_t>(), "start block height")
    ("end-block", po::value<uint64_t>(), "end block height")
    ("version", po::value<std::string>()->default_value("0.0.1"), "x.x.x")
    ("handle", po::value<std::string>(), "request handle")
    ("height", po::value<uint64_t>()->default_value(0), "request height")
    ("msg", po::value<std::string>(), "experiment message")
    ("rpc-listen", po::value<std::string>()->default_value("127.0.0.1"), "nipc listen")
    ("rpc-port", po::value<uint16_t>()->default_value(0x1958), "nipc port");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    exit(1);
  }

  return vm;
}

class cli_executor {
public:
  cli_executor(const std::string &rpc_listen, uint16_t rpc_port)
      : m_rpc_listen(rpc_listen), m_rpc_port(rpc_port) {}

  void send_nr_req(uint64_t start_block, uint64_t end_block, uint64_t version) {
    std::shared_ptr<nbre_nr_handle_req> req =
        std::make_shared<nbre_nr_handle_req>();
    req->set<p_holder>(reinterpret_cast<uint64_t>(this));
    req->set<p_start_block>(start_block);
    req->set<p_end_block>(end_block);
    req->set<p_nr_version>(version);
    m_package = req;
    start_and_join();
  }
  void send_nr_result_req(const std::string &handle) {
    std::shared_ptr<nbre_nr_result_by_handle_req> req =
        std::make_shared<nbre_nr_result_by_handle_req>();
    req->set<p_holder>(reinterpret_cast<uint64_t>(this));
    req->set<p_nr_handle>(handle);
    m_package = req;
    start_and_join();
  }

  void send_nr_result_by_height_req(uint64_t height) {
    std::shared_ptr<nbre_nr_result_by_height_req> req =
        std::make_shared<nbre_nr_result_by_height_req>();
    req->set<p_holder>(reinterpret_cast<uint64_t>(this));
    req->set<p_height>(height);
    m_package = req;
    start_and_join();
  }

  void send_nr_sum_req(uint64_t height) {
    std::shared_ptr<nbre_nr_sum_req> req = std::make_shared<nbre_nr_sum_req>();
    req->set<p_holder>(reinterpret_cast<uint64_t>(this));
    req->set<p_height>(height);
    m_package = req;
    start_and_join();
  }

  void send_dip_reward_req(uint64_t height, uint64_t version) {
    std::shared_ptr<nbre_dip_reward_req> req =
        std::make_shared<nbre_dip_reward_req>();
    req->set<p_holder>(reinterpret_cast<uint64_t>(this));
    req->set<p_height>(height);
    req->set<p_version>(version);
    m_package = req;
    start_and_join();
  }

  void send_experiment_req(uint64_t version, const std::string &msg) {
    auto req = std::make_shared<nbre_experiment_req>();
    req->set<p_holder>(reinterpret_cast<uint64_t>(this));
    req->set<p_version>(version);
    req->set<p_msg>(msg);
    m_package = req;
    start_and_join();
  }
  void send_lib_req(uint64_t version, const std::string &msg) {
    auto req = std::make_shared<nbre_lib_req>();
    req->set<p_holder>(reinterpret_cast<uint64_t>(this));
    req->set<p_version>(version);
    req->set<p_msg>(msg);
    m_package = req;
    start_and_join();
  }

protected:
  void start_and_join() {

    ff::net::net_nervure nn;

    ff::net::typed_pkg_hub hub;
    ff::net::tcp_connection_base_ptr conn;
    nn.get_event_handler()->listen<::ff::net::event::tcp_get_connection>(
        [&, this](::ff::net::tcp_connection_base *conn) {
          conn->send(m_package);
        });

    hub.to_recv_pkg<nbre_nr_result_by_handle_ack>(
        [&](std::shared_ptr<nbre_nr_result_by_handle_ack> ack) {
          std::cout << "\t " << ack->get<p_nr_result>() << std::endl;
          conn->close();
          exit(-1);
        });
    hub.to_recv_pkg<nbre_nr_handle_ack>(
        [&](std::shared_ptr<nbre_nr_handle_ack> ack) {
          std::cout << "\t" << ack->get<p_nr_handle>() << std::endl;
          conn->close();
          exit(-1);
        });

    hub.to_recv_pkg<nbre_nr_result_by_height_ack>(
        [&](std::shared_ptr<nbre_nr_result_by_height_ack> ack) {
          std::cout << "\t " << ack->get<p_nr_result>() << std::endl;
          conn->close();
          exit(-1);
        });
    hub.to_recv_pkg<nbre_nr_sum_ack>([&](std::shared_ptr<nbre_nr_sum_ack> ack) {
      std::cout << "\t " << ack->get<p_nr_sum>() << std::endl;
      conn->close();
      exit(-1);
    });
    hub.to_recv_pkg<nbre_dip_reward_ack>(
        [&](std::shared_ptr<nbre_dip_reward_ack> ack) {
          std::cout << "\t" << ack->get<p_dip_reward>() << std::endl;
          conn->close();
          exit(-1);
        });
    hub.to_recv_pkg<nbre_experiment_ack>(
        [&](std::shared_ptr<nbre_experiment_ack> ack) {
          std::cout << "\t" << ack->get<p_msg>() << std::endl;
          conn->close();
          exit(-1);
        });
    hub.to_recv_pkg<nbre_lib_ack>([&](std::shared_ptr<nbre_lib_ack> ack) {
      std::cout << "\t" << ack->get<p_status>() << std::endl;
      conn->close();
      exit(-1);
    });
    nn.add_pkg_hub(hub);
    conn = nn.add_tcp_client(m_rpc_listen, m_rpc_port);

    nn.run();
  }

protected:
  std::shared_ptr<ff::net::package> m_package;
  std::string m_rpc_listen;
  uint16_t m_rpc_port;
};

int main(int argc, char *argv[]) {
  po::variables_map vm = get_variables_map(argc, argv);
  std::string rpc_listen = vm["rpc-listen"].as<std::string>();
  uint16_t rpc_port = vm["rpc-port"].as<uint16_t>();

  if (vm.count("query")) {
    std::string type = vm["query"].as<std::string>();
    if (type != "nr" && type != "nr-result" && type != "nr-sum" &&
        type != "dip-reward" && type != "exp" && type != "lib") {
      std::cout << "invalid type " << type << std::endl;
      exit(-1);
    }
    LOG(INFO) << "query for " << type;
    if (type == "nr") {
      if (!vm.count("start-block") || !vm.count("end-block") ||
          !vm.count("version")) {
        std::cout << "no start, end block, or version" << std::endl;
        exit(-1);
      }
      auto start_block = vm["start-block"].as<uint64_t>();
      auto end_block = vm["end-block"].as<uint64_t>();
      auto version_str = vm["version"].as<std::string>();
      neb::version v;
      v.from_string(version_str);
      cli_executor ce(rpc_listen, rpc_port);
      ce.send_nr_req(start_block, end_block, v.data());
    }
    if (type == "nr-result") {
      if (vm.count("handle")) {
        auto handle = vm["handle"].as<std::string>();
        cli_executor ce(rpc_listen, rpc_port);
        ce.send_nr_result_req(handle);
      } else if (vm.count("height")) {
        auto height = vm["height"].as<uint64_t>();
        LOG(INFO) << "cli query nr-result by height " << height;
        cli_executor ce(rpc_listen, rpc_port);
        ce.send_nr_result_by_height_req(height);
      }
    }
    if (type == "nr-sum") {
      if (vm.count("height")) {
        auto height = vm["height"].as<uint64_t>();
        LOG(INFO) << "cli query nr-sum by height " << height;
        cli_executor ce(rpc_listen, rpc_port);
        ce.send_nr_sum_req(height);
      }
    }
    if (type == "dip-reward") {
      auto height = vm["height"].as<uint64_t>();
      auto version_str = vm["version"].as<std::string>();
      neb::version v;
      v.from_string(version_str);
      cli_executor ce(rpc_listen, rpc_port);
      ce.send_dip_reward_req(height, v.data());
    }
    if (type == "exp") {
      auto msg = vm["msg"].as<std::string>();
      auto version_str = vm["version"].as<std::string>();
      neb::version v;
      v.from_string(version_str);
      cli_executor ce(rpc_listen, rpc_port);
      ce.send_experiment_req(v.data(), msg);
    }
    if (type == "lib") {
      auto msg = vm["msg"].as<std::string>();
      auto version_str = vm["version"].as<std::string>();
      neb::version v;
      v.from_string(version_str);
      cli_executor ce(rpc_listen, rpc_port);
      ce.send_lib_req(v.data(), msg);
    }
  }
  return 0;
}
