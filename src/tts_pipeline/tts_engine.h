#ifndef VOICEVOX_ENGINE_TTS_PIPELINE
#define VOICEVOX_ENGINE_TTS_PIPELINE
#include <cstdint>
#include <string>
#include <tuple>
#include <map>
#include "../core/core_wrapper.h"
#include "../core/core_adapter.h"
#include "../defines.h"
#include "../core/core_initializer.h"
#include "./phoneme.h"
#include "./model.h"
#include "../utility/model.h"
#include "../libraries/AudioFile.h"

std::vector<Mora> to_flatten_moras(std::vector<AccentPhrase> *accent_phrases);
std::vector<Phoneme> to_flatten_phonemes(std::vector<Mora> *moras);
std::vector<int64_t> create_one_hot(AccentPhrase *accent_phrase, int32_t index);
Mora generate_silence_mora(float length);
std::vector<AccentPhrase> apply_interrogative_upspeak(std::vector<AccentPhrase> *accent_phrases, bool enable_interrogative_upspeak);
std::vector<Mora> apply_prepost_silence(std::vector<Mora> *moras, AudioQuery *query);
std::vector<Mora> apply_speed_scale(std::vector<Mora> *moras, AudioQuery *query);
std::pair<std::vector<int64_t>, std::vector<int64_t>> count_frame_per_unit(std::vector<Mora> *moras);
int32_t to_frame(float sec);
std::vector<Mora> apply_pitch_scale(std::vector<Mora> *moras, AudioQuery *query);
std::vector<Mora> apply_pause_length(std::vector<Mora> *moras, AudioQuery *query);
std::vector<Mora> apply_pause_length_scale(std::vector<Mora> *moras, AudioQuery *query);
std::vector<Mora> apply_intonation_scale(std::vector<Mora> *moras, AudioQuery *query);
AudioFile<float> apply_volume_scale(AudioFile<float> *wave, AudioQuery *query);
AudioFile<float> apply_volume_scale(AudioFile<float> *wave, FrameAudioQuery *query);
AudioFile<float> apply_output_sampling_rate(AudioFile<float> *wave, float sr_wave, AudioQuery *query);
AudioFile<float> apply_output_sampling_rate(AudioFile<float> *wave, float sr_wave, FrameAudioQuery *query);
AudioFile<float> apply_output_stereo(AudioFile<float> *wave, AudioQuery *query);
AudioFile<float> apply_output_stereo(AudioFile<float> *wave, FrameAudioQuery *query);
std::pair<WaveLike, std::vector<float>> query_to_decoder_feature(AudioQuery *query);
AudioFile<float> raw_wave_to_output_wave(AudioQuery *query, AudioFile<float> *wave, int32_t sr_wave);
AudioFile<float> raw_wave_to_output_wave(FrameAudioQuery *query, AudioFile<float> *wave, int32_t sr_wave);
std::string hira_to_kana(std::string const &text);
std::vector<int64_t> calc_phoneme_lengths(std::vector<int64_t> *consonant_lengths, std::vector<int64_t> *note_durations);
std::tuple<
    std::vector<int64_t>,
    std::vector<int64_t>,
    std::vector<int64_t>,
    std::vector<int64_t>,
    std::vector<int64_t>,
    std::vector<NoteId>>
notes_to_keys_and_phonemes(std::vector<Note> *notes);
std::tuple<std::vector<int64_t>, std::vector<float>, std::vector<float>> frame_query_to_sf_decoder_feature(FrameAudioQuery *query);

class TTSEngine
{
public:
  void initialize(CoreWrapper *core);
  int32_t default_sampling_rate = 0;
  DeviceSupport supported_devices = DeviceSupport();
  std::vector<AccentPhrase> update_length(std::vector<AccentPhrase> *accent_phrases, StyleId style_id);
  std::vector<AccentPhrase> update_pitch(std::vector<AccentPhrase> *accent_phrases, StyleId style_id);
  std::vector<AccentPhrase> update_length_and_pitch(std::vector<AccentPhrase> *accent_phrases, StyleId style_id);
  std::vector<AccentPhrase> create_accent_phrases(std::string const &text, StyleId style_id);
  std::vector<AccentPhrase> create_accent_phrases_from_kana(std::string const &kana, StyleId style_id);
  AudioFile<float> synthesize_wave(AudioQuery *query, StyleId style_id, bool enable_interrogative_upspeak = true);
  void initialize_synthesis(StyleId style_id, bool skip_reinit);
  bool is_synthesis_initialized(StyleId style_id);
  std::tuple<std::vector<FramePhoneme>, std::vector<float>, std::vector<float>> create_sing_phoneme_and_f0_and_volume(Score *score, StyleId style_id);
  std::vector<float> create_sing_volume_from_phoneme_and_f0(Score *score, std::vector<FramePhoneme> *phonemes, std::vector<float> *f0s, StyleId style_id);
  AudioFile<float> frame_synthsize_wave(FrameAudioQuery *query, StyleId style_id);

private:
  CoreAdapter *core = new CoreAdapter();
};
class TTSEngineManager
{
public:
  std::vector<std::string> versions() const;
  void register_engine(TTSEngine &engine, std::string const &version);
  TTSEngine get_engine(); // latest
  TTSEngine get_engine(std::string const &version);

private:
  std::string latest_version();
  std::map<std::string, TTSEngine> engines = std::map<std::string, TTSEngine>();
};
TTSEngineManager make_tts_engines_from_cores(CoreManager *core_manager);
#endif
