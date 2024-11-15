#include <thread>
#include <iostream>
#include <iostream>
#include "./core_initializer.h"
#include "../utility/core_version_utility.h"
#include "../utility/path_utility.h"
#include "../libraries/json.hpp"
#include "../util.h"

using json = nlohmann::json;

std::vector<std::string> CoreManager::versions() const
{
  std::vector<std::string> keys;
  for (auto &&core : cores)
  {
    keys.emplace_back(std::string(core.first));
  }
  return keys;
}

std::string CoreManager::latest_version() const
{
  std::vector<std::string> versions = this->versions();
  return get_latest_version(&versions);
}

void CoreManager::register_core(CoreAdapter &core, std::string const &version)
{
  this->cores.emplace(std::string(version), core);
}

CoreAdapter CoreManager::get_core(std::string const &version)
{
  return this->cores.at(version);
}

bool CoreManager::has_core(std::string const &version) const
{
  return this->cores.count(version) > 0;
}

std::vector<std::pair<std::string, CoreAdapter>> CoreManager::items()
{
  std::vector<std::pair<std::string, CoreAdapter>> items;
  for (auto &&core : cores)
  {
    items.emplace_back(std::string(core.first), core.second);
  }
  return items;
}

int32_t get_half_logical_cores()
{
  int32_t logical_cores = static_cast<int32_t>(std::thread::hardware_concurrency());
  return logical_cores / 2;
}

CoreManager initialize_cores(bool use_gpu, std::vector<std::string> *voicelib_dirs, std::string const &voicevox_dir, std::vector<std::string> *runtime_dirs, int32_t cpu_num_threads, bool enable_mock, bool load_all_models)
{
  std::vector<std::string> runtime_dirs_copy = copy_strings(runtime_dirs);
  std::vector<std::string> voicelib_dirs_copy = copy_strings(voicelib_dirs);
  if (cpu_num_threads == 0)
  {
    std::cerr << "Warning: cpu_num_threads is set to 0. "
                 "Setting it to half of the logical cores."
              << std::endl;
    cpu_num_threads = get_half_logical_cores();
  }
  std::string root_dir = engine_root();
  if (!voicevox_dir.empty())
    runtime_dirs_copy.emplace_back(voicevox_dir);
  if (runtime_dirs_copy.size() == 0)
    runtime_dirs_copy.emplace_back(root_dir);
  load_runtime_lib(&runtime_dirs_copy);
  CoreManager core_manager;
  if (!voicevox_dir.empty())
    voicelib_dirs_copy.emplace_back(voicevox_dir);
  if (voicelib_dirs_copy.size() == 0)
    voicelib_dirs_copy.emplace_back(root_dir);
  if (!enable_mock)
  {
    auto load_core_library = [&core_manager, &use_gpu, &cpu_num_threads, &load_all_models](std::string const &core_dir, bool suppress_error)
    {
      try
      {
        CoreAdapter core_adapter;
        core_adapter.initialize(use_gpu, core_dir, cpu_num_threads, load_all_models);
        json metas = json::parse(core_adapter.core->metas());
        std::string core_version = metas.at(0)["version"];
        std::cout << "Info: Loading core " + core_version + "." << std::endl;
        if (core_manager.has_core(core_version))
        {
          std::cerr << "Warning: Core loading is skipped because of version duplication." << std::endl;
        }
        else
        {
          core_manager.register_core(core_adapter, core_version);
        }
      }
      catch (const std::string &e)
      {
        if (!suppress_error)
          throw e;
      }
    };
    for (auto &&voicelib_dir : voicelib_dirs_copy)
    {
      load_core_library(voicelib_dir, false);
    }
    std::vector<std::string> user_voicelib_dirs;
    std::filesystem::path core_libraries_dir = std::filesystem::path(get_save_dir()) / "core_libraries";
    std::error_code e;
    std::filesystem::create_directories(core_libraries_dir, e);
    user_voicelib_dirs.emplace_back(std::string(core_libraries_dir));
    for (auto &&file : std::filesystem::directory_iterator(core_libraries_dir))
    {
      if (file.is_regular_file())
        user_voicelib_dirs.emplace_back(file.path());
    }
    for (auto &&core_dir : user_voicelib_dirs)
    {
      load_core_library(core_dir, true);
    }
  }
  else
  {
    // mock
  }
  return core_manager;
}
