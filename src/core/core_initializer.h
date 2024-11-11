#ifndef VOICEVOX_ENGINE_CORE_INITIALIZER
#define VOICEVOX_ENGINE_CORE_INITIALIZER
#include <cstdint>
#include <vector>
#include <string>
#include <tuple>
#include <map>
#include "./core_adapter.h"

int32_t get_half_logical_cores();
class CoreManager
{
public:
  std::vector<std::string> versions() const;
  std::string latest_version() const;
  // coreはclass内で保存
  void register_core(CoreAdapter &core, std::string const &version);
  CoreAdapter get_core(std::string const &version);
  bool has_core(std::string const &version) const;
  std::vector<std::pair<std::string, CoreAdapter>> items();
private:
  std::map<std::string, CoreAdapter> cores = std::map<std::string, CoreAdapter>();
};
int32_t get_half_logical_cores();
CoreManager initialize_cores(bool use_gpu, std::vector<std::string> *voicelib_dirs, std::string const &voicevox_dir, std::vector<std::string> *runtime_dirs, int32_t cpu_num_threads, bool enable_mock, bool load_all_models);
#endif
