#include <vector>
#include <string>
#include <filesystem>
#include <map>
#include <cstdint>
#include "../defines.h"
#include "../util.h"
#include "./core_wrapper.h"

#ifdef _WIN32
#include <windows.h>
#define DLL HMODULE
DLL dll_load(const char *path)
{
  return LoadLibraryA(path);
}

void dll_free(DLL &dll)
{
  FreeLibrary(dll);
  return;
}

template <typename T>
T load_func(DLL &dll, const char *func_name)
{
  if (dll == NULL)
  {
    std::cout << "dll_not_found" << std::endl;
    throw "dll_not_loaded";
  }
  void *fn = GetProcAddress(dll, func_name);
  if (fn == NULL)
  {
    std::cout << "fn_not_found" << std::endl;
    throw "fn_not_found";
  }
  return reinterpret_cast<T>(fn);
}
#else
#include <dlfcn.h>
#include <sys/utsname.h>
#define DLL void *
#endif

DLL load_core_dll(std::string const &core_dll_path)
{
  DLL dll;
#ifdef _WIN32
  dll = LoadLibraryA(path);
  if (dll == NULL)
    throw GetLastError();
#else
  dll = dlopen(core_dll_path.c_str(), RTLD_NOW);
  if (dll == NULL)
    throw dlerror();
#endif
  return dll;
}
void load_dll(std::string const &dll_path)
{
  DLL dll;
#ifdef _WIN32
  LoadLibraryExA(path, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);
#else
  dll = dlopen(dll_path.c_str(), RTLD_NOW | RTLD_GLOBAL);
#endif
}

template <typename T>
T load_func(DLL dll, const char *func_name)
{
  void *fn;
#ifdef _WIN32
  dll = GetProcAddress(dll, func_name);
  if (dll == NULL)
    throw GetLastError();
#else
  fn = dlsym(dll, func_name);
  if (dll == NULL)
    throw dlerror();
#endif
  return reinterpret_cast<T>(fn);
}

void load_runtime_lib(std::vector<std::string> *runtime_dirs)
{
  std::vector<std::filesystem::path> lib_file_names;
  std::vector<std::filesystem::path> lib_names;
#ifdef _WIN32
  lib_file_names.emplace_back(std::filesystem::path("torch_cpu.dll"));
  lib_file_names.emplace_back(std::filesystem::path("torch_cuda.dll"));
  lib_file_names.emplace_back(std::filesystem::path("DirectML.dll"));
  lib_file_names.emplace_back(std::filesystem::path("onnxruntime.dll"));
  lib_names.emplace_back(std::filesystem::path("torch_cpu"));
  lib_names.emplace_back(std::filesystem::path("torch_cuda"));
  lib_names.emplace_back(std::filesystem::path("onnxruntime"));
#elif __MACH__
  lib_file_names.emplace_back(std::filesystem::path("libonnxruntime.dylib"));
#elif __linux__
  lib_file_names.emplace_back(std::filesystem::path("libtorch.so"));
  lib_file_names.emplace_back(std::filesystem::path("libonnxruntime.so"));
  lib_names.emplace_back(std::filesystem::path("torch"));
  lib_names.emplace_back(std::filesystem::path("onnxruntime"));
  lib_names.emplace_back(std::filesystem::path("onnxruntime"));
#else
  throw "不明なOSです";
#endif
  for (auto &&runtime_dir : *runtime_dirs)
  {
    for (auto &&lib_file_name : lib_file_names)
    {
      std::filesystem::path path = std::filesystem::path(runtime_dir) / lib_file_name;
      load_dll(path);
    }
  }
  for (auto &&lib_name : lib_names)
  {
    load_dll(lib_name);
  }
}

std::string find_version_0_12_core_or_later(std::string const &core_dir)
{
  std::map<std::string, std::pair<std::string, std::string>> corename_dict;
  corename_dict.emplace("Windows", std::make_pair("voicevox_core.dll", "core.dll"));
  corename_dict.emplace("Linux", std::make_pair("libvoicevox_core.so", "libcore.so"));
  corename_dict.emplace("Darwin", std::make_pair("libvoicevox_core.dylib", "libcore.dylib"));

  std::filesystem::path dir(core_dir);
  if (std::filesystem::exists(dir / "metas.json"))
  {
    return std::string();
  }
  std::pair<std::string, std::string> sys = corename_dict.at(platform_system());

  if (std::filesystem::exists(dir / sys.first) && (std::filesystem::is_regular_file(dir / sys.first) || std::filesystem::is_symlink(dir / sys.first)))
    return sys.first;
  if (std::filesystem::exists(dir / sys.second) && (std::filesystem::is_regular_file(dir / sys.second) || std::filesystem::is_symlink(dir / sys.second)))
    return sys.second;
  return std::string();
}

std::string get_arch_name()
{
#ifdef _WIN32 // なんとかしたい
#ifdef _WIN64 // 64bit
#ifdef _M_X64 // x64
  return std::string("x64");
#else // arm 64bit
  return std::string("aarch64");
#endif
#else         // 32bit
#ifdef _M_ARM // arm 32bit
  return std::string("armv7l");
#else         // x86
  return std::string("x86");
#endif
#endif
#else
  utsname buf;
  uname(&buf);
  std::string machine = buf.machine;
  if (machine == "x86_64" || machine == "x64" || machine == "AMD64")
    return "x64";
  else if (machine == "i386" or machine == "x86")
    return "x86";
  else if (machine == "arm64")
    return "aarch64";
  else if (machine == "aarch64")
    return "aarch64";
  else if (machine == "armv7l")
    return "armv7l";
  else
    return std::string();
#endif
}

std::string get_core_name(std::string const &arch_name, std::string const &platform_name, std::string const &model_type, GPUType gpu_type)
{
  std::string arch_name_copy(arch_name);
  if (platform_name == "Darwin")
  {
    if (gpu_type == GPUType::NONE && (arch_name_copy == "x64" || arch_name_copy == "aarch64"))
    {
      arch_name_copy = "universal";
    }
    else
    {
      return std::string();
    }
  }
  std::vector<CoreInfo> core_infos;
  if (true) // core_infos
  {
    core_infos.emplace_back(CoreInfo{std::string("core.dll"), std::string("Windows"), std::string("x64"), std::string("libtorch"), GPUType::CUDA});
    core_infos.emplace_back(CoreInfo{std::string("core_cpu.dll"), std::string("Windows"), std::string("x64"), std::string("libtorch"), GPUType::NONE});
    core_infos.emplace_back(CoreInfo{std::string("core_gpu_x64_nvidia.dll"), std::string("Windows"), std::string("x64"), std::string("libtorch"), GPUType::CUDA});
    core_infos.emplace_back(CoreInfo{std::string("core_gpu_x64_directml.dll"), std::string("Windows"), std::string("x64"), std::string("libtorch"), GPUType::DIRECT_ML});
    core_infos.emplace_back(CoreInfo{std::string("core_cpu_x64.dll"), std::string("Windows"), std::string("x64"), std::string("onnxruntime"), GPUType::NONE});
    core_infos.emplace_back(CoreInfo{std::string("core_cpu_x86.dll"), std::string("Windows"), std::string("x86"), std::string("onnxruntime"), GPUType::NONE});
    core_infos.emplace_back(CoreInfo{std::string("core_gpu_x86_directml.dll"), std::string("Windows"), std::string("x86"), std::string("onnxruntime"), GPUType::DIRECT_ML});
    core_infos.emplace_back(CoreInfo{std::string("core_cpu_arm.dll"), std::string("Windows"), std::string("armv7l"), std::string("onnxruntime"), GPUType::NONE});
    core_infos.emplace_back(CoreInfo{std::string("core_gpu_arm_directml.dll"), std::string("Windows"), std::string("armv7l"), std::string("onnxruntime"), GPUType::DIRECT_ML});
    core_infos.emplace_back(CoreInfo{std::string("core_cpu_arm64.dll"), std::string("Windows"), std::string("aarch64"), std::string("onnxruntime"), GPUType::NONE});
    core_infos.emplace_back(CoreInfo{std::string("core_gpu_arm64_directml.dll"), std::string("Windows"), std::string("aarch64"), std::string("onnxruntime"), GPUType::DIRECT_ML});

    core_infos.emplace_back(CoreInfo{std::string("libcore.so"), std::string("Linux"), std::string("x64"), std::string("libtorch"), GPUType::CUDA});
    core_infos.emplace_back(CoreInfo{std::string("libcore_cpu.so"), std::string("Linux"), std::string("x64"), std::string("libtorch"), GPUType::NONE});
    core_infos.emplace_back(CoreInfo{std::string("libcore_gpu_x64_nvidia.so"), std::string("Linux"), std::string("x64"), std::string("onnxruntime"), GPUType::CUDA});
    core_infos.emplace_back(CoreInfo{std::string("libcore_cpu_x64.so"), std::string("Linux"), std::string("x64"), std::string("onnxruntime"), GPUType::NONE});
    core_infos.emplace_back(CoreInfo{std::string("libcore_cpu_armhf.so"), std::string("Linux"), std::string("armv7l"), std::string("onnxruntime"), GPUType::NONE});
    core_infos.emplace_back(CoreInfo{std::string("libcore_cpu_arm64.so"), std::string("Linux"), std::string("aarch64"), std::string("onnxruntime"), GPUType::NONE});

    core_infos.emplace_back(CoreInfo{std::string("libcore_cpu_universal2.dylib"), std::string("Darwin"), std::string("universal"), std::string("onnxruntime"), GPUType::NONE});
  }
  for (auto &&core_info : core_infos)
  {
    if (
        core_info.platform == platform_name && core_info.arch == arch_name_copy && core_info.core_type == model_type && core_info.gpu_type == gpu_type)
    {
      return core_info.name;
    }
  }

  return std::string();
}

std::string get_suitable_core_name(std::string const &model_type, GPUType gpu_type)
{
  std::string arch_name = get_arch_name();
  std::string platform_name = platform_system();
  if (arch_name == "")
    return std::string();
  return get_core_name(arch_name, platform_name, model_type, gpu_type);
}

std::string check_core_type(std::string const &core_dir)
{
  std::vector<std::string> libtorch_core_names;
  libtorch_core_names.emplace_back(get_suitable_core_name("libtorch", GPUType::CUDA));
  libtorch_core_names.emplace_back(get_suitable_core_name("libtorch", GPUType::NONE));
  std::vector<std::string> onnxruntime_core_names;
  onnxruntime_core_names.emplace_back(get_suitable_core_name("onnxruntime", GPUType::CUDA));
  onnxruntime_core_names.emplace_back(get_suitable_core_name("onnxruntime", GPUType::DIRECT_ML));
  onnxruntime_core_names.emplace_back(get_suitable_core_name("onnxruntime", GPUType::NONE));
  for (auto &&name : libtorch_core_names)
  {
    if (name != "" && std::filesystem::is_regular_file(std::filesystem::path(core_dir) / name))
      return "libtorch";
  }
  for (auto &&name : onnxruntime_core_names)
  {
    if (name != "" && std::filesystem::is_regular_file(std::filesystem::path(core_dir) / name))
      return "onnxruntime";
  }
  return std::string();
}

DLL load_core(std::string const &core_dir, bool use_gpu)
{
  std::string core_name = find_version_0_12_core_or_later(core_dir);
  if (!core_name.empty())
  {
    return load_core_dll(std::filesystem::path(core_dir) / core_name);
  }
  std::string model_type = check_core_type(core_dir);
  if (!model_type.empty())
    throw "コアが見つかりません";
  if (use_gpu || model_type == "onnxruntime")
  {
    core_name = get_suitable_core_name(model_type, GPUType::CUDA);
    try
    {
      return load_core_dll(std::filesystem::path(core_dir) / core_name);
    }
    catch (const std::exception &e)
    {
      core_name = get_suitable_core_name(model_type, GPUType::DIRECT_ML);
      try
      {
        return load_core_dll(std::filesystem::path(core_dir) / core_name);
      }
      catch (const std::exception &e)
      {
      }
    }
  }
  core_name = get_suitable_core_name(model_type, GPUType::NONE);
  if (!core_name.empty())
  {
    try
    {
      return load_core_dll(std::filesystem::path(core_dir) / core_name);
    }
    catch (const std::exception &e)
    {
      if (model_type == "libtorch")
      {
        core_name = get_suitable_core_name(model_type, GPUType::CUDA);
        if (!core_name.empty())
        {
          try
          {
            return load_core_dll(std::filesystem::path(core_dir) / core_name);
          }
          catch (const std::exception &e)
          {
          }
        }
      }
      throw "コアの読み込みに失敗しました";
    }
  }
#ifdef _WIN32
  throw "このコンピュータのアーキテクチャ" + get_arch_name() + "で利用可能なコアがありません";
#else
  utsname buf;
  uname(&buf);
  std::string machine = buf.machine;
  throw "このコンピュータのアーキテクチャ" + machine + "で利用可能なコアがありません";
#endif
}

void CoreWrapper::initialize(bool use_gpu, std::string const &core_path, uint32_t cpu_num_threads, bool load_all_models)
{
  this->default_sampling_rate = 24000;
  this->core = load_core_dll(core_path);
  assert_core_success(load_func<bool (*)(bool, int32_t, bool)>(this->core, "initialize")(use_gpu, cpu_num_threads, load_all_models));
}

CoreWrapper::~CoreWrapper()
{
#ifdef _WIN32
  FreeLibrary(this->core);
#else
  dlclose(this->core);
#endif
}

bool CoreWrapper::is_initialized() const
{
  return this->core != nullptr;
}

const char *CoreWrapper::metas()
{
  return load_func<const char *(*)(void)>(this->core, "metas")();
}

// `output`は長さ`length`で初期化されている必要がある
bool CoreWrapper::yukarin_s_forward(int64_t length, int64_t *phoneme_list, StyleId *style_id, float *output)
{
  return assert_core_success(load_func<bool (*)(int64_t, int64_t *, int64_t *, float *)>(this->core, "yukarin_s_forward")(length, phoneme_list, style_id, output));
}

// `output`は長さ`length`で初期化されている必要がある
bool CoreWrapper::yukarin_sa_forward(int64_t length, int64_t *vowel_phoneme_list, int64_t *consonant_phoneme_list, int64_t *start_accent_list, int64_t *end_accent_list, int64_t *start_accent_phrase, int64_t *end_accent_phrase, StyleId *style_id, float *output)
{
  return assert_core_success(load_func<bool (*)(int64_t, int64_t *, int64_t *, int64_t *, int64_t *, int64_t *, int64_t *, int64_t *, float *)>(this->core, "yukarin_sa_forward")(length, vowel_phoneme_list, consonant_phoneme_list, start_accent_list, end_accent_list, start_accent_phrase, end_accent_phrase, style_id, output));
}

// `output`は長さ`length*256`で初期化されている必要がある
bool CoreWrapper::decode_forward(int64_t length, int64_t phoneme_size, float *f0, float *phoneme, StyleId *style_id, float *output)
{
  return assert_core_success(load_func<bool (*)(int64_t, int64_t, float *, float *, int64_t *, float *)>(this->core, "decode_forward")(length, phoneme_size, f0, phoneme, style_id, output));
}

// `output`は長さ`length`で初期化されている必要がある
bool CoreWrapper::predict_sing_consonant_length_forward(int64_t length, int64_t *consonant, int64_t *vowel, int64_t *note_duration, StyleId *style_id, int64_t *output)
{
  return assert_core_success(load_func<bool (*)(int64_t, int64_t *, int64_t *, int64_t *, int64_t *, int64_t *)>(this->core, "predict_sing_consonant_length_forward")(length, consonant, vowel, note_duration, style_id, output));
}

// `output`は長さ`length`で初期化されている必要がある
bool CoreWrapper::predict_sing_f0_forward(int64_t length, int64_t *phoneme, int64_t *note, StyleId *style_id, float *output)
{
  return assert_core_success(load_func<bool (*)(int64_t, int64_t *, int64_t *, int64_t *, float *)>(this->core, "predict_sing_f0_forward")(length, phoneme, note, style_id, output));
}

// `output`は長さ`length`で初期化されている必要がある
bool CoreWrapper::predict_sing_volume_forward(int64_t length, int64_t *phoneme, int64_t *note, float *f0, StyleId *style_id, float *output)
{
  return assert_core_success(load_func<bool (*)(int64_t, int64_t *, int64_t *, float *, int64_t *, float *)>(this->core, "predict_sing_volume_forward")(length, phoneme, note, f0, style_id, output));
}

// `output`は長さ`length*256`で初期化されている必要がある
bool CoreWrapper::sf_decode_forward(int64_t length, int64_t *phoneme, float *f0, float *volume, StyleId *style_id, float *output)
{
  return assert_core_success(load_func<bool (*)(int64_t, int64_t *, float *, float *, int64_t *, float *)>(this->core, "sf_decode_forward")(length, phoneme, f0, volume, style_id, output));
}

const char *CoreWrapper::supported_devices()
{
  return load_func<const char *(*)()>(this->core, "supported_devices")();
}

void CoreWrapper::finalize()
{
  return load_func<void (*)()>(this->core, "finalize")();
}

bool CoreWrapper::load_model(StyleId style_id)
{
  return assert_core_success(load_func<bool (*)(int64_t)>(this->core, "load_model")(style_id));
}

bool CoreWrapper::is_model_loaded(StyleId style_id)
{
  return load_func<bool (*)(int64_t)>(this->core, "is_model_loaded")(style_id);
}

bool CoreWrapper::assert_core_success(bool result)
{
  if (result == false)
  {
    this->last_error_message = const_cast<char *>(load_func<const char *(*)(void)>(this->core, "last_error_message")());
  }
  return result;
}
