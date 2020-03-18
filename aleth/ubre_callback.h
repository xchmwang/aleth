
#pragma once
#include "common.h"

class callback_handler : public neb::util::singleton<callback_handler> {
public:
  callback_handler();

  template <typename Func> void add_version_handler(uint64_t holder, Func &&f) {
    std::unique_lock<std::mutex> _l(m_mutex);
    m_version_handlers.insert(std::make_pair(holder, f));
  }
  void handle_version(void *holder, uint32_t major, uint32_t minor,
                      uint32_t patch);

  template <typename Func> void add_ir_list_handler(uint64_t holder, Func &&f) {
    std::unique_lock<std::mutex> _l(m_mutex);
    m_ir_list_handlers.insert(std::make_pair(holder, f));
  }
  void handle_ir_list(void *holder, const char *ir_name_list);

  template <typename Func>
  void add_ir_versions_handler(uint64_t holder, Func &&f) {
    std::unique_lock<std::mutex> _l(m_mutex);
    m_ir_versions_handlers.insert(std::make_pair(holder, f));
  }
  void handle_ir_versions(void *holder, const char *ir_versions);

  template <typename Func> void add_nr_handler(uint64_t holder, Func &&f) {
    std::unique_lock<std::mutex> _l(m_mutex);
    m_nr_handlers.insert(std::make_pair(holder, f));
  }
  void handle_nr(void *holder, const char *nr_handler_id);

  template <typename Func>
  void add_nr_result_handler(uint64_t holder, Func &&f) {
    std::unique_lock<std::mutex> _l(m_mutex);
    m_nr_result_handlers.insert(std::make_pair(holder, f));
  }
  void handle_nr_result(void *holder, const char *nr_result);

  template <typename Func>
  void add_nr_result_by_height_handler(uint64_t holder, Func &&f) {
    std::unique_lock<std::mutex> _l(m_mutex);
    m_nr_result_by_height_handlers.insert(std::make_pair(holder, f));
  }
  void handle_nr_result_by_height(void *holder, const char *nr_result);

  template <typename Func> void add_nr_sum_handler(uint64_t holder, Func &&f) {
    std::unique_lock<std::mutex> _l(m_mutex);
    m_nr_sum_handlers.insert(std::make_pair(holder, f));
  }
  void handle_nr_sum(void *holder, const char *nr_sum);

  template <typename Func>
  void add_dip_reward_handler(uint64_t holder, Func &&f) {
    std::unique_lock<std::mutex> _l(m_mutex);
    m_dip_reward_handlers.insert(std::make_pair(holder, f));
  }
  void handle_dip_reward(void *holder, const char *dip_reward);

  template <typename Func>
  void add_experiment_handler(uint64_t holder, Func &&f) {
    std::unique_lock<std::mutex> _l(m_mutex);
    m_experiment_handlers.insert(std::make_pair(holder, f));
  }
  void handle_experiment(void *holder, const char *ret);

  template <typename Func> void add_lib_handler(uint64_t holder, Func &&f) {
    std::unique_lock<std::mutex> _l(m_mutex);
    m_lib_handlers.insert(std::make_pair(holder, f));
  }
  void handle_lib(void *holder, int32_t ret);

protected:
  template <typename Handlers, typename... ARGS>
  void handle(Handlers &hs, void *holder, ARGS... args) {
    std::unique_lock<std::mutex> _l(m_mutex);
    auto h = reinterpret_cast<uint64_t>(holder);
    auto it = hs.find(h);
    if (it == hs.end()) {
      // LOG(INFO) << "can't find handler";
      return;
    }
    it->second(h, args...);
    hs.erase(it);
  }

protected:
  std::mutex m_mutex;

  std::unordered_map<
      uint64_t, std::function<void(uint64_t, uint32_t, uint32_t, uint32_t)>>
      m_version_handlers;

  std::unordered_map<uint64_t, std::function<void(uint64_t, const char *)>>
      m_ir_list_handlers;

  std::unordered_map<uint64_t, std::function<void(uint64_t, const char *)>>
      m_ir_versions_handlers;

  std::unordered_map<uint64_t, std::function<void(uint64_t, const char *)>>
      m_nr_handlers;

  std::unordered_map<uint64_t, std::function<void(uint64_t, const char *)>>
      m_nr_result_handlers;

  std::unordered_map<uint64_t, std::function<void(uint64_t, const char *)>>
      m_nr_result_by_height_handlers;

  std::unordered_map<uint64_t, std::function<void(uint64_t, const char *)>>
      m_nr_sum_handlers;

  std::unordered_map<uint64_t, std::function<void(uint64_t, const char *)>>
      m_dip_reward_handlers;

  std::unordered_map<uint64_t, std::function<void(uint64_t, const char *)>>
      m_experiment_handlers;
  std::unordered_map<uint64_t, std::function<void(uint64_t, int32_t)>>
      m_lib_handlers;
};

void nbre_version_callback(ipc_status_code isc, void *handler, uint32_t major,
                           uint32_t minor, uint32_t patch);

void nbre_ir_list_callback(ipc_status_code isc, void *handler,
                           const char *ir_name_list);
void nbre_ir_versions_callback(ipc_status_code isc, void *handler,
                               const char *ir_versions);

void nbre_nr_handle_callback(ipc_status_code isc, void *holder,
                             const char *nr_handle_id);

void nbre_nr_result_callback(ipc_status_code isc, void *holder,
                             const char *nr_result);

void nbre_nr_result_by_height_callback(ipc_status_code isc, void *holder,
                                       const char *nr_result);

void nbre_nr_sum_callback(ipc_status_code isc, void *holder,
                          const char *nr_sum);

void nbre_dip_reward_callback(ipc_status_code isc, void *holder,
                              const char *dip_reward);

void nbre_experiment_callback(ipc_status_code isc, void *holder,
                              const char *ret);
void nbre_lib_callback(ipc_status_code isc, void *holder, int32_t ret);
