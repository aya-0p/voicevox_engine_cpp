#ifndef VOICEVOX_ENGINE_CORE_ADAPTER
#define VOICEVOX_ENGINE_CORE_ADAPTER
#include <cstdint>
#include <string>
#include <vector>
#include "./core_wrapper.h"
#include "../metas/metas.h"

typedef struct CoreCharacterStyle
{
  std::string name = "";
  int32_t id = 0;
  std::string type = "";
} CoreCharacterStyle;

typedef struct CoreCharacter
{
  std::string name = "";
  std::string speaker_uuid = "";
  std::vector<CoreCharacterStyle> *styles = new std::vector<CoreCharacterStyle>();
  std::string version = "";
} CoreCharacter;

typedef struct DeviceSupport
{
  bool cpu = false;
  bool cuda = false;
  bool dml = false;
} DeviceSupport;

class CoreAdapter
{
public:
  void initialize(bool use_gpu, std::string const &core_dir, uint32_t cpu_num_threads, bool load_all_models);
  bool is_initialized() const;
  std::vector<CoreCharacter> characters();
  DeviceSupport supported_devices();
  bool initialize_style_id_synthesis(StyleId style_id, bool skip_reinit);
  bool is_initialized_style_id_synthesis(StyleId style_id);
  std::vector<float> safe_yukarin_s_forward(std::vector<int64_t> *phoneme_list, StyleId style_id);
  std::vector<float> safe_yukarin_sa_forward(
      std::vector<int64_t> *vowel_phoneme_list,
      std::vector<int64_t> *consonant_phoneme_list,
      std::vector<int64_t> *start_accent_list,
      std::vector<int64_t> *end_accent_list,
      std::vector<int64_t> *start_accent_phrase,
      std::vector<int64_t> *end_accent_phrase,
      StyleId style_id);
  std::pair<std::vector<std::vector<float>>, int32_t> safe_decode_forward(
      std::vector<std::vector<float>> *phoneme,
      std::vector<float> *f0,
      StyleId style_id);
  std::vector<int64_t> safe_predict_sing_consonant_length_forward(
      std::vector<int64_t> *consonant,
      std::vector<int64_t> *vowel,
      std::vector<int64_t> *note_duration,
      StyleId style_id);
  std::vector<float> safe_predict_sing_f0_forward(
      std::vector<int64_t> *phoneme,
      std::vector<int64_t> *note,
      StyleId style_id);
  std::vector<float> safe_predict_sing_volume_forward(
      std::vector<int64_t> *phoneme,
      std::vector<int64_t> *note,
      std::vector<float> *f0,
      StyleId style_id);
  std::pair<std::vector<std::vector<float>>, int32_t> safe_sf_decode_forward(
      std::vector<int64_t> *phoneme,
      std::vector<float> *f0,
      std::vector<float> *volume,
      StyleId style_id);
  std::string get_last_error_message() const;

  int32_t default_sampling_rate = 0;
  CoreWrapper *core = new CoreWrapper();
};
#endif
