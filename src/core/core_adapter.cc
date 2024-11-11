#include <vector>
#include "./core_adapter.h"
#include "./core_wrapper.h"
#include "../libraries/json.hpp"
#include "../util.h"
#include "../defines.h"

using json = nlohmann::json;

void CoreAdapter::initialize(bool use_gpu, std::string const &core_dir, uint32_t cpu_num_threads, bool load_all_models = false)
{
  this->core->initialize(use_gpu, core_dir, cpu_num_threads, load_all_models);
  this->default_sampling_rate = core->default_sampling_rate;
}

bool CoreAdapter::is_initialized() const
{
  return this->core->is_initialized();
}

std::vector<CoreCharacter> CoreAdapter::characters()
{
  std::vector<CoreCharacter> characters;
  const char *json_text = this->core->metas();
  json json_obj = json::parse(json_text);
  for (size_t i = 0; i < json_obj.size(); i++)
  {
    json obj = json_obj.at(i);
    std::string name = obj["name"];
    std::string speaker_uuid = obj["spealer_uuid"];
    std::vector<CoreCharacterStyle> styles;
    std::string version = obj["version"];
    json styles_obj = obj["styles"];
    for (size_t j = 0; j < styles_obj.size(); j++)
    {
      json obj2 = styles_obj.at(j);
      std::string name = obj2["name"];
      int32_t id = obj2["id"];
      std::string type = obj2["type"];
      CoreCharacterStyle style = {name, id, type};
      styles.emplace_back(style);
    }
    CoreCharacter character;
    character.name = name;
    character.speaker_uuid = speaker_uuid;
    character.styles;
    for (auto &&style : styles)
    {
      character.styles->emplace_back(style);
    }
    character.version = version;
    characters.emplace_back(character);
  }
  return characters;
}

DeviceSupport CoreAdapter::supported_devices()
{
  const char *json_text = this->core->supported_devices();
  json json_obj = json::parse(json_text);
  bool cpu = json_obj["cpu"];
  bool cuda = json_obj["cuda"];
  bool dml = json_obj["dml"];
  DeviceSupport device_support = {cpu, cuda, dml};
  return device_support;
}

bool CoreAdapter::initialize_style_id_synthesis(StyleId style_id, bool skip_reinit)
{
  if (skip_reinit == false || this->core->is_model_loaded(style_id) == false)
  {
    return this->core->load_model(style_id);
  }
  return false;
}

bool CoreAdapter::is_initialized_style_id_synthesis(StyleId style_id)
{
  return this->core->is_model_loaded(style_id);
}

std::vector<float> CoreAdapter::safe_yukarin_s_forward(std::vector<int64_t> *phoneme_list, StyleId style_id)
{
  this->initialize_style_id_synthesis(style_id, true);
  int64_t length = phoneme_list->size() + 2;
  int64_t *phoneme_list_data = reinterpret_cast<int64_t *>(malloc((phoneme_list->size() + 2) * sizeof(int64_t)));
  vec2arr_int64_t(phoneme_list, (int64_t)0, (int64_t)0, phoneme_list_data);
  float *output = reinterpret_cast<float *>(calloc(length, sizeof(float)));
  bool result = this->core->yukarin_s_forward(length, phoneme_list_data, &style_id, output);
  std::vector<float> output_data;
  free(phoneme_list_data);
  if (result == true)
  {
    output_data = arr2vec_float(output, length);
  }
  free(output);
  return output_data;
}

std::vector<float> CoreAdapter::safe_yukarin_sa_forward(std::vector<int64_t> *vowel_phoneme_list, std::vector<int64_t> *consonant_phoneme_list, std::vector<int64_t> *start_accent_list, std::vector<int64_t> *end_accent_list, std::vector<int64_t> *start_accent_phrase, std::vector<int64_t> *end_accent_phrase, StyleId style_id)
{
  this->initialize_style_id_synthesis(style_id, true);
  int64_t length = vowel_phoneme_list->size() + 2;
  int64_t *vowel_phoneme_list_data = reinterpret_cast<int64_t *>(malloc(sizeof(int64_t) * (vowel_phoneme_list->size() + 2)));
  vec2arr_int64_t(vowel_phoneme_list, int64_t(0), int64_t(0), vowel_phoneme_list_data);
  int64_t *consonant_phoneme_list_data = reinterpret_cast<int64_t *>(malloc(sizeof(int64_t) * (consonant_phoneme_list->size() + 2)));
  vec2arr_int64_t(consonant_phoneme_list, int64_t(-1), int64_t(-1), consonant_phoneme_list_data);
  int64_t *start_accent_list_data = reinterpret_cast<int64_t *>(malloc(sizeof(int64_t) * (start_accent_list->size() + 2)));
  vec2arr_int64_t(start_accent_list, int64_t(0), int64_t(0), start_accent_list_data);
  int64_t *end_accent_list_data = reinterpret_cast<int64_t *>(malloc(sizeof(int64_t) * (end_accent_list->size() + 2)));
  vec2arr_int64_t(end_accent_list, int64_t(0), int64_t(0), end_accent_list_data);
  int64_t *start_accent_phrase_data = reinterpret_cast<int64_t *>(malloc(sizeof(int64_t) * (start_accent_phrase->size() + 2)));
  vec2arr_int64_t(start_accent_phrase, int64_t(0), int64_t(0), start_accent_phrase_data);
  int64_t *end_accent_phrase_data = reinterpret_cast<int64_t *>(malloc(sizeof(int64_t) * (end_accent_phrase->size() + 2)));
  vec2arr_int64_t(end_accent_phrase, int64_t(0), int64_t(0), end_accent_phrase_data);
  float *output = reinterpret_cast<float *>(calloc(length, sizeof(float)));
  bool result = this->core->yukarin_sa_forward(length, vowel_phoneme_list_data, consonant_phoneme_list_data, start_accent_list_data, end_accent_list_data, start_accent_phrase_data, end_accent_phrase_data, &style_id, output);
  std::vector<float> output_data;
  free(vowel_phoneme_list_data);
  free(consonant_phoneme_list_data);
  free(start_accent_list_data);
  free(end_accent_list_data);
  free(start_accent_phrase_data);
  free(end_accent_phrase_data);
  if (result == true)
  {
    output_data = arr2vec_float(output, length);
  }
  free(output);
  return output_data;
}

std::pair<std::vector<std::vector<float>>, int32_t> CoreAdapter::safe_decode_forward(std::vector<std::vector<float>> *phoneme, std::vector<float> *f0, StyleId style_id)
{
  int64_t length = phoneme->size();
  int64_t phoneme_size = (*phoneme)[0].size();
  this->initialize_style_id_synthesis(style_id, true);
  float *f0_data = reinterpret_cast<float *>(malloc(sizeof(float) * f0->size()));
  vec2arr_float(f0, f0_data);
  float *phoneme_data = reinterpret_cast<float *>(malloc(sizeof(float) * length * phoneme_size));
  for (size_t i = 0; i < length; i++)
  {
    for (size_t j = 0; j < phoneme_size; j++)
    {
      phoneme_data[i * phoneme_size + j] = (*phoneme)[i][j];
    }
  }
  float *output = reinterpret_cast<float *>(calloc(length * 256, sizeof(float)));
  bool result = this->core->decode_forward(length, phoneme_size, f0_data, phoneme_data, &style_id, output);
  std::vector<std::vector<float>> output_data;
  free(f0_data);
  free(phoneme_data);
  if (result == true)
  {
    output_data.emplace_back(arr2vec_float(output, length * 256));
  }
  free(output);
  return {output_data, this->default_sampling_rate};
}

std::vector<int64_t> CoreAdapter::safe_predict_sing_consonant_length_forward(std::vector<int64_t> *consonant, std::vector<int64_t> *vowel, std::vector<int64_t> *note_duration, StyleId style_id)
{
  this->initialize_style_id_synthesis(style_id, true);
  int64_t length = consonant->size();
  int64_t *consonant_data = reinterpret_cast<int64_t *>(malloc(sizeof(int64_t) * consonant->size()));
  vec2arr_int64_t(consonant, consonant_data);
  int64_t *vowel_data = reinterpret_cast<int64_t *>(malloc(sizeof(int64_t) * vowel->size()));
  vec2arr_int64_t(vowel, vowel_data);
  int64_t *note_duration_data = reinterpret_cast<int64_t *>(malloc(sizeof(int64_t) * note_duration->size()));
  vec2arr_int64_t(note_duration, note_duration_data);
  int64_t *output = reinterpret_cast<int64_t *>(calloc(length, sizeof(int64_t)));
  bool result = this->core->predict_sing_consonant_length_forward(length, consonant_data, vowel_data, note_duration_data, &style_id, output);
  std::vector<int64_t> output_data;
  free(consonant_data);
  free(vowel_data);
  free(note_duration_data);
  if (result == true)
  {
    output_data = arr2vec_int64_t(output, length);
  }
  free(output);
  return output_data;
}

std::vector<float> CoreAdapter::safe_predict_sing_f0_forward(std::vector<int64_t> *phoneme, std::vector<int64_t> *note, StyleId style_id)
{
  int64_t length = phoneme->size();
  int64_t *phoneme_data = reinterpret_cast<int64_t *>(malloc(sizeof(int64_t) * phoneme->size()));
  vec2arr_int64_t(phoneme, phoneme_data);
  int64_t *note_data = reinterpret_cast<int64_t *>(malloc(sizeof(int64_t) * note->size()));
  vec2arr_int64_t(note, note_data);
  float *output = reinterpret_cast<float *>(calloc(length, sizeof(float)));
  bool result = this->core->predict_sing_f0_forward(length, phoneme_data, note_data, &style_id, output);
  std::vector<float> output_data;
  free(phoneme_data);
  free(note_data);
  if (result == true)
  {
    output_data = arr2vec_float(output, length);
  }
  free(output);
  return output_data;
}

std::vector<float> CoreAdapter::safe_predict_sing_volume_forward(std::vector<int64_t> *phoneme, std::vector<int64_t> *note, std::vector<float> *f0, StyleId style_id)
{
  int64_t length = phoneme->size();
  int64_t *phoneme_data = reinterpret_cast<int64_t *>(malloc(sizeof(int64_t) * phoneme->size()));
  vec2arr_int64_t(phoneme, phoneme_data);
  int64_t *note_data = reinterpret_cast<int64_t *>(malloc(sizeof(int64_t) * note->size()));
  vec2arr_int64_t(note, note_data);
  float *f0_data = reinterpret_cast<float *>(malloc(sizeof(float) * f0->size()));
  vec2arr_float(f0, f0_data);
  float *output = reinterpret_cast<float *>(calloc(length, sizeof(float)));
  bool result = this->core->predict_sing_volume_forward(length, phoneme_data, note_data, f0_data, &style_id, output);
  std::vector<float> output_data;
  free(phoneme_data);
  free(note_data);
  free(f0_data);
  if (result == true)
  {
    output_data = arr2vec_float(output, length);
  }
  free(output);
  return output_data;
}

std::pair<std::vector<std::vector<float>>, int32_t> CoreAdapter::safe_sf_decode_forward(std::vector<int64_t> *phoneme, std::vector<float> *f0, std::vector<float> *volume, StyleId style_id)
{
  int64_t length = phoneme->size();
  int64_t *phoneme_data = reinterpret_cast<int64_t *>(malloc(sizeof(int64_t) * phoneme->size()));
  vec2arr_int64_t(phoneme, phoneme_data);
  float *f0_data = reinterpret_cast<float *>(malloc(sizeof(float) * f0->size()));
  vec2arr_float(f0, f0_data);
  float *volume_data = reinterpret_cast<float *>(malloc(sizeof(float) * volume->size()));
  vec2arr_float(volume, volume_data);
  float *output = reinterpret_cast<float *>(calloc(length * 256, sizeof(float)));
  bool result = this->core->sf_decode_forward(length, phoneme_data, f0_data, volume_data, &style_id, output);
  std::vector<std::vector<float>> output_data;
  free(phoneme_data);
  free(f0_data);
  free(volume_data);
  if (result == true)
  {
    output_data.emplace_back(arr2vec_float(output, length * 256));
  }
  free(output);
  return {output_data, this->default_sampling_rate};
}

std::string CoreAdapter::get_last_error_message() const
{
  return std::string(this->core->last_error_message);
}
