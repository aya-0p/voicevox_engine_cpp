#ifndef VOICEVOX_ENGINE_CORE_WEAPPER
#define VOICEVOX_ENGINE_CORE_WEAPPER
#include <cstdint>
#include <string>
#include <vector>
#include "../defines.h"

#ifdef _WIN32
#define DLL HMODULE
#else
#define DLL void *
#endif

enum GPUType
{
  NONE = 0,
  CUDA = 1,
  DIRECT_ML = 2,
};

void load_runtime_lib(std::vector<std::string> *runtime_dirs);
std::string find_version_0_12_core_or_later(std::string const &core_dir);
std::string get_arch_name();
std::string get_core_name(std::string const &arch_name, std::string const &platform_name, std::string const &model_type, GPUType gpu_type);
std::string get_suitable_core_name(std::string const &model_type, GPUType gpu_type);
std::string check_core_type(std::string const &core_dir);
DLL load_core(std::string const &core_dir, bool use_gpu);

typedef struct CoreInfo
{
  std::string name = "";
  std::string platform = "";
  std::string arch = "";
  std::string core_type = "";
  GPUType gpu_type = GPUType::NONE;
} CoreInfo;

class CoreWrapper
{
public:
  ~CoreWrapper();

  void initialize(bool use_gpu, std::string const &core_dir, uint32_t cpu_num_threads, bool load_all_models = false);
  const char *metas();
  bool yukarin_s_forward(int64_t length, int64_t *phoneme_list, StyleId *style_id, float *output);
  bool yukarin_sa_forward(
      int64_t length,
      int64_t *vowel_phoneme_list,
      int64_t *consonant_phoneme_list,
      int64_t *start_accent_list,
      int64_t *end_accent_list,
      int64_t *start_accent_phrase,
      int64_t *end_accent_phrase,
      StyleId *style_id,
      float *output);
  bool decode_forward(
      int64_t length,
      int64_t phoneme_size,
      float *f0,
      float *phoneme,
      StyleId *style_id,
      float *output);
  bool predict_sing_consonant_length_forward(
      int64_t length,
      int64_t *consonant,
      int64_t *vowel,
      int64_t *note_duration,
      StyleId *style_id,
      int64_t *output);
  bool predict_sing_f0_forward(
      int64_t length,
      int64_t *phoneme,
      int64_t *note,
      StyleId *style_id,
      float *output);
  bool predict_sing_volume_forward(
      int64_t length,
      int64_t *phoneme,
      int64_t *note,
      float *f0,
      StyleId *style_id,
      float *output);
  bool sf_decode_forward(
      int64_t length,
      int64_t *phoneme,
      float *f0,
      float *volume,
      StyleId *style_id,
      float *output);
  const char *supported_devices();
  void finalize();
  bool load_model(StyleId style_id);
  bool is_model_loaded(StyleId style_id);
  bool is_initialized() const;

  char *last_error_message;
  int32_t default_sampling_rate = 0;
  DLL core = nullptr;

private:
  bool assert_core_success(bool result);
};
#endif
