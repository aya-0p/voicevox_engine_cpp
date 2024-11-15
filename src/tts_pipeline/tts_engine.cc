#include "./tts_engine.h"
#include "../core/core_adapter.h"
#include "./model.h"
#include "./mora_mapping.h"
#include "../util.h"
#include "../libraries/AudioFile.h"
#include "./text_analyzer.h"
#include "./kana_converter.h"
#include <math.h>
#include "../utility/core_version_utility.h"

double UpspeakLength = 0.15;
double UpspeakPitchAdd = 0.3;
double UpspeakPitchMax = 6.5;

std::vector<Mora> to_flatten_moras(std::vector<AccentPhrase> *accent_phrases)
{
  std::vector<Mora> moras;
  for (auto &&accent_phrase : *accent_phrases)
  {
    for (auto &&mora : *accent_phrase.moras)
    {
      moras.emplace_back(mora);
    }
    if (accent_phrase.pause_mora != nullptr)
      moras.emplace_back(*accent_phrase.pause_mora);
  }
  return moras;
}

std::vector<Phoneme> to_flatten_phonemes(std::vector<Mora> *moras)
{
  std::vector<Phoneme> phonemes;
  for (auto &&mora : *moras)
  {
    if (!mora.consonant.empty())
    {
      Phoneme phoneme;
      phoneme.initialize(mora.consonant);
      phonemes.emplace_back(phoneme);
    }
    Phoneme phoneme;
    phoneme.initialize(mora.vowel);
    phonemes.emplace_back(phoneme);
  }
  return phonemes;
}

std::vector<int64_t> create_one_hot(AccentPhrase *accent_phrase, int32_t index)
{
  std::vector<int64_t> onehot(accent_phrase->moras->size(), 0);
  onehot[index] = 1;
  if (accent_phrase->pause_mora != nullptr)
    onehot.emplace_back(0);
  return onehot;
}

Mora generate_silence_mora(float length)
{
  Mora mora;
  mora.text = "　";
  mora.consonant = "";
  mora.consonant_length = -1;
  mora.vowel = "sil";
  mora.vowel_length = length;
  mora.pitch = 0;
  return mora;
}

std::vector<AccentPhrase> apply_interrogative_upspeak(std::vector<AccentPhrase> *accent_phrases, bool enable_interrogative_upspeak)
{
  if (!enable_interrogative_upspeak)
    return *accent_phrases;
  for (auto &&accent_phrase : *accent_phrases)
  {
    std::vector<Mora> moras = *accent_phrase.moras;
    if (moras.size() != 0 && accent_phrase.is_interrogative && moras[moras.size() - 1].pitch > 0)
    {
      Mora last_mora = copy_mora(&moras[moras.size() - 1]);
      Mora upspeak_mora;
      upspeak_mora.text = std::string(mora_phonemes_to_mora_kana.at(last_mora.vowel));
      upspeak_mora.consonant = "";
      upspeak_mora.consonant_length = -1;
      upspeak_mora.vowel = last_mora.vowel;
      upspeak_mora.vowel_length = last_mora.vowel_length;
      upspeak_mora.pitch = std::min(last_mora.pitch + UpspeakPitchAdd, UpspeakPitchMax);
      accent_phrase.moras->emplace_back(upspeak_mora);
    }
  }
  return *accent_phrases;
}

std::vector<Mora> apply_prepost_silence(std::vector<Mora> *moras, AudioQuery *query)
{
  std::vector<Mora> pre_silence_moras = {generate_silence_mora(query->prePhonemeLength)};
  std::vector<Mora> post_silence_moras = {generate_silence_mora(query->postPhonemeLength)};
  std::vector<Mora> moras_new;
  for (auto &&mora : pre_silence_moras)
  {
    moras_new.emplace_back(mora);
  }
  for (auto &&mora : *moras)
  {
    moras_new.emplace_back(mora);
  }
  for (auto &&mora : post_silence_moras)
  {
    moras_new.emplace_back(mora);
  }
  return *moras;
}

std::vector<Mora> apply_speed_scale(std::vector<Mora> *moras, AudioQuery *query)
{
  for (auto &&mora : *moras)
  {
    mora.vowel_length /= query->speedScale;
    if (mora.consonant_length != -1)
      mora.consonant_length /= query->speedScale;
  }
  return *moras;
}

std::pair<std::vector<int64_t>, std::vector<int64_t>> count_frame_per_unit(std::vector<Mora> *moras)
{
  std::vector<int64_t> frame_per_phoneme;
  std::vector<int64_t> frame_per_mora;
  for (auto &&mora : *moras)
  {
    int64_t vowel_frames = to_frame(mora.vowel_length);
    int64_t consonant_frames = mora.consonant_length != -1 ? to_frame(mora.consonant_length) : 0;
    int64_t mora_frames = vowel_frames + consonant_frames;
    if (mora.consonant_length != -1)
      frame_per_phoneme.emplace_back(consonant_frames);
    frame_per_phoneme.emplace_back(vowel_frames);
    frame_per_mora.emplace_back(mora_frames);
  }
  return {frame_per_phoneme, frame_per_mora};
}

int32_t to_frame(float sec)
{
  const int32_t Framerate = 93.75; // = 24000 / 256 [frame/sec]
  return static_cast<int32_t>(std::round(sec * Framerate / 2)) * 2;
}

std::vector<Mora> apply_pitch_scale(std::vector<Mora> *moras, AudioQuery *query)
{
  for (auto &&mora : *moras)
  {
    mora.pitch *= std::pow(2, query->pitchScale);
  }
  return *moras;
}

std::vector<Mora> apply_pause_length(std::vector<Mora> *moras, AudioQuery *query)
{
  if (query->pauseLength != -1)
  {
    for (auto &&mora : *moras)
    {
      if (mora.vowel == "pau")
        mora.vowel_length = query->pauseLength;
    }
  }
  return *moras;
}

std::vector<Mora> apply_pause_length_scale(std::vector<Mora> *moras, AudioQuery *query)
{
  for (auto &&mora : *moras)
  {
    if (mora.vowel == "pau")
      mora.vowel_length *= query->pauseLengthScale;
  }
  return *moras;
}

std::vector<Mora> apply_intonation_scale(std::vector<Mora> *moras, AudioQuery *query)
{
  double voiced_sum = 0;
  uint32_t voiced_count = 0;
  for (auto &&mora : *moras)
  {
    if (mora.pitch > 0)
    {
      voiced_sum += mora.pitch;
      voiced_count++;
    }
  }
  voiced_sum /= voiced_count;
  for (auto &&mora : *moras)
  {
    if (mora.pitch > 0)
    {
      mora.pitch = static_cast<float>((static_cast<double>(mora.pitch) - voiced_sum) * query->intonationScale + voiced_sum);
    }
  }
  return *moras;
}

AudioFile<float> apply_volume_scale(AudioFile<float> *wave, AudioQuery *query)
{
  for (size_t i = 0; i < wave->samples.size(); i++)
  {
    for (size_t j = 0; j < wave->samples[i].size(); j++)
    {
      wave->samples[i][j] *= query->volumeScale;
    }
  }
  return *wave;
}

AudioFile<float> apply_volume_scale(AudioFile<float> *wave, FrameAudioQuery *query)
{
  for (size_t i = 0; i < wave->samples.size(); i++)
  {
    for (size_t j = 0; j < wave->samples[i].size(); j++)
    {
      wave->samples[i][j] *= query->volumeScale;
    }
  }
  return *wave;
}

AudioFile<float> apply_output_sampling_rate(AudioFile<float> *wave, float sr_wave, AudioQuery *query)
{
  wave->setSampleRate(query->outputSamplingRate);
  return *wave;
}

AudioFile<float> apply_output_sampling_rate(AudioFile<float> *wave, float sr_wave, FrameAudioQuery *query)
{
  wave->setSampleRate(query->outputSamplingRate);
  return *wave;
}

AudioFile<float> apply_output_stereo(AudioFile<float> *wave, AudioQuery *query)
{
  if (query->outputStereo)
    wave->setNumChannels(2);
  return *wave;
}

AudioFile<float> apply_output_stereo(AudioFile<float> *wave, FrameAudioQuery *query)
{
  if (query->outputStereo)
    wave->setNumChannels(2);
  return *wave;
}

std::pair<WaveLike, std::vector<float>> query_to_decoder_feature(AudioQuery *query)
{
  std::vector<Mora> moras = to_flatten_moras(query->accent_phrases);
  moras = apply_prepost_silence(&moras, query);
  moras = apply_pause_length(&moras, query);
  moras = apply_pause_length_scale(&moras, query);
  moras = apply_speed_scale(&moras, query);
  moras = apply_pitch_scale(&moras, query);
  moras = apply_intonation_scale(&moras, query);

  std::vector<Phoneme> phoneme_temp = to_flatten_phonemes(&moras);
  std::vector<std::vector<float>> phoneme = std::vector<std::vector<float>>(phoneme_temp.size(), std::vector<float>());
  for (size_t i = 0; i < phoneme_temp.size(); i++)
  {
    phoneme[i] = phoneme_temp[i].onehot();
  }
  std::vector<float> f0 = std::vector<float>(moras.size(), 0);
  for (size_t i = 0; i < moras.size(); i++)
  {
    f0[i] = moras[i].pitch;
  }
  auto [frame_per_phoneme, frame_per_mora] = count_frame_per_unit(&moras);
  std::vector<std::vector<float>> phoneme_fix = numpy_repeat(&phoneme, &frame_per_phoneme);
  std::vector<float> f0_fix = numpy_repeat(&f0, &frame_per_mora);
  return {phoneme, f0};
}

AudioFile<float> raw_wave_to_output_wave(AudioQuery *query, AudioFile<float> *wave, int32_t sr_wave)
{
  *wave = apply_volume_scale(wave, query);
  *wave = apply_output_sampling_rate(wave, sr_wave, query);
  *wave = apply_output_stereo(wave, query);
  return *wave;
}

AudioFile<float> raw_wave_to_output_wave(FrameAudioQuery *query, AudioFile<float> *wave, int32_t sr_wave)
{
  *wave = apply_volume_scale(wave, query);
  *wave = apply_output_sampling_rate(wave, sr_wave, query);
  *wave = apply_output_stereo(wave, query);
  return *wave;
}

std::string hira_to_kana(std::string const &text)
{
  // #error 後でやる（UTF-8）
  return std::string();
}

std::vector<int64_t> calc_phoneme_lengths(std::vector<int64_t> *consonant_lengths, std::vector<int64_t> *note_durations)
{
  std::vector<int64_t> phoneme_durations;
  for (size_t i = 0; i < consonant_lengths->size(); i++)
  {
    if (i < consonant_lengths->size() - 1)
    {
      if (i == 0 && (*consonant_lengths)[i] != 0)
        throw "onsonant_lengths[0] must be 0";
      int64_t next_consonant_length = (*consonant_lengths)[i + 1];
      int64_t note_duration = (*note_durations)[i];
      if (next_consonant_length < 0)
        next_consonant_length = (*consonant_lengths)[i + 1] = note_duration / 2;
      int64_t vowel_length = note_duration - next_consonant_length;
      if (vowel_length < 0)
      {
        next_consonant_length = (*consonant_lengths)[i + 1] = note_duration / 2;
        vowel_length = note_duration - next_consonant_length;
      }
      phoneme_durations.emplace_back(vowel_length);
      if (next_consonant_length > 0)
        phoneme_durations.emplace_back(next_consonant_length);
    }
    else
    {
      int64_t vowel_length = (*note_durations)[i];
      phoneme_durations.emplace_back(vowel_length);
    }
  }
  return phoneme_durations;
}

std::tuple<std::vector<int64_t>, std::vector<int64_t>, std::vector<int64_t>, std::vector<int64_t>, std::vector<int64_t>, std::vector<NoteId>> notes_to_keys_and_phonemes(std::vector<Note> *notes)
{
  std::vector<int64_t> note_lengths;
  std::vector<int64_t> note_consonants;
  std::vector<int64_t> note_vowels;
  std::vector<int64_t> phonemes;
  std::vector<int64_t> phoneme_keys;
  std::vector<std::string> phoneme_note_ids;
  for (auto &&note : *notes)
  {
    if (note.lyric == "")
    {
      if (note.key != -1)
        throw "lyricが空文字列の場合、keyはnullである必要があります。";
      note_lengths.emplace_back(note.frame_length);
      note_consonants.emplace_back(-1);
      note_vowels.emplace_back(0);
      phonemes.emplace_back(0);
      phoneme_keys.emplace_back(-1);
      phoneme_note_ids.emplace_back(note.id);
    }
    else
    {
      std::string consonant;
      std::string vowel;
      if (note.key == -1)
        throw "keyがnullの場合、lyricは空文字列である必要があります。";
      if (mora_kana_to_mora_phonemes.count(note.lyric) > 0)
      {
        auto [consonant_, vowel_] = mora_kana_to_mora_phonemes.at(note.lyric);
        consonant = consonant_;
        vowel_ = vowel;
      }
      else
      {
        if (mora_kana_to_mora_phonemes.count(hira_to_kana(note.lyric)) > 0)
        {
          auto [consonant_, vowel_] = mora_kana_to_mora_phonemes.at(hira_to_kana(note.lyric));
          consonant = consonant_;
          vowel_ = vowel;
        }
        else
          throw "lyricが不正です";
      }
      Phoneme phoneme_temp;
      int32_t consonant_id = consonant == "" ? -1 : Phoneme::id(consonant);
      int32_t vowel_id = Phoneme::id(vowel);
      note_lengths.emplace_back(note.frame_length);
      note_consonants.emplace_back(consonant_id);
      note_vowels.emplace_back(vowel_id);
      if (consonant_id != -1)
      {
        phonemes.emplace_back(consonant_id);
        phoneme_keys.emplace_back(note.key);
        phoneme_note_ids.emplace_back(note.id);
      }
      phonemes.emplace_back(vowel_id);
      phoneme_keys.emplace_back(note.key);
      phoneme_note_ids.emplace_back(note.id);
    }
  }
  return {note_lengths, note_consonants, note_vowels, phonemes, phoneme_keys, phoneme_note_ids};
}

std::tuple<std::vector<int64_t>, std::vector<float>, std::vector<float>> frame_query_to_sf_decoder_feature(FrameAudioQuery *query)
{
  std::vector<int64_t> phonemes;
  std::vector<int64_t> phoneme_lengths;
  for (auto &&phoneme : *query->phonemes)
  {
    if (!vector_has(&PhonemeList, phoneme.phoneme))
      throw "phoneme is not valid";
    phonemes.emplace_back(Phoneme::id(phoneme.phoneme));
    phoneme_lengths.emplace_back(phoneme.frame_length);
  }
  std::vector<int64_t> frame_phonemes = numpy_repeat(&phonemes, &phoneme_lengths);
  std::vector<float> f0s(*query->f0);
  std::vector<float> volumes(*query->volume);
  return {frame_phonemes, f0s, volumes};
}

void TTSEngine::initialize(CoreWrapper *core)
{
  this->core->core = core;
  this->default_sampling_rate = this->core->default_sampling_rate;
  this->supported_devices = this->core->supported_devices();
}

std::vector<AccentPhrase> TTSEngine::update_length(std::vector<AccentPhrase> *accent_phrases, StyleId style_id)
{
  std::vector<Mora> moras = to_flatten_moras(accent_phrases);
  std::vector<Phoneme> phonemes = to_flatten_phonemes(&moras);
  std::vector<int64_t> phoneme_ids = std::vector<int64_t>(phonemes.size(), 0);
  for (size_t i = 0; i < phonemes.size(); i++)
  {
    phoneme_ids[i] = phonemes[i].id();
  }
  std::vector<float> phoneme_lengths = this->core->safe_yukarin_s_forward(&phoneme_ids, style_id);
  std::vector<int32_t> vowel_indexes;
  for (size_t i = 0; i < phonemes.size(); i++)
  {
    Phoneme phoneme = phonemes[i];
    if (phoneme.is_mora_tail())
      vowel_indexes.emplace_back(i);
  }
  for (size_t i = 0; i < moras.size(); i++)
  {
    if (moras[i].consonant == "")
      moras[i].consonant_length = 0;
    else
      moras[i].consonant_length = phoneme_lengths[vowel_indexes[i] - 1];
    moras[i].vowel_length = phoneme_lengths[vowel_indexes[i]];
  }
  return *accent_phrases;
}

std::vector<AccentPhrase> TTSEngine::update_pitch(std::vector<AccentPhrase> *accent_phrases, StyleId style_id)
{
  if (accent_phrases->size() == 0)
    return std::vector<AccentPhrase>();
  std::vector<std::vector<int64_t>> one_hots_temp_1;
  for (auto &&accent_phrase : *accent_phrases)
  {
    one_hots_temp_1.emplace_back(create_one_hot(&accent_phrase, accent_phrase.accent == 1 ? 0 : 1));
  }
  std::vector<int64_t> start_accent_list = numpy_concatenate(&one_hots_temp_1);
  std::vector<std::vector<int64_t>> one_hots_temp_2;
  for (auto &&accent_phrase : *accent_phrases)
  {
    one_hots_temp_2.emplace_back(create_one_hot(&accent_phrase, accent_phrase.accent - 1));
  }
  std::vector<int64_t> end_accent_list = numpy_concatenate(&one_hots_temp_2);
  std::vector<std::vector<int64_t>> one_hots_temp_3;
  for (auto &&accent_phrase : *accent_phrases)
  {
    one_hots_temp_3.emplace_back(create_one_hot(&accent_phrase, 0));
  }
  std::vector<int64_t> start_accent_phrase_list = numpy_concatenate(&one_hots_temp_3);
  std::vector<std::vector<int64_t>> one_hots_temp_4;
  for (auto &&accent_phrase : *accent_phrases)
  {
    one_hots_temp_4.emplace_back(create_one_hot(&accent_phrase, -1));
  }
  std::vector<int64_t> end_accent_phrase_list = numpy_concatenate(&one_hots_temp_4);
  std::vector<Mora> moras = to_flatten_moras(accent_phrases);
  std::vector<int64_t> consonant_ids;
  for (auto &&mora : moras)
  {
    consonant_ids.emplace_back(mora.consonant.empty() ? -1 : Phoneme::id(mora.consonant));
  }
  std::vector<Phoneme> vowels;
  for (auto &&mora : moras)
  {
    Phoneme phoneme;
    phoneme.initialize(mora.vowel);
    vowels.emplace_back(phoneme);
  }
  std::vector<int64_t> vowel_ids;
  for (auto &&p : vowels)
  {
    vowel_ids.emplace_back(p.id());
  }
  std::vector<float> f0 = this->core->safe_yukarin_sa_forward(
      &vowel_ids,
      &consonant_ids,
      &start_accent_list,
      &end_accent_list,
      &start_accent_phrase_list,
      &end_accent_phrase_list,
      style_id);
  for (size_t i = 0; i < vowels.size(); i++)
  {
    if (vowels[i].is_unvoiced_mora_tail())
      f0[i] = 0;
  }
  for (size_t i = 0; i < moras.size(); i++)
  {
    moras[i].pitch = f0[i];
  }
  return *accent_phrases;
}

std::vector<AccentPhrase> TTSEngine::update_length_and_pitch(std::vector<AccentPhrase> *accent_phrases, StyleId style_id)
{
  *accent_phrases = this->update_length(accent_phrases, style_id);
  *accent_phrases = this->update_pitch(accent_phrases, style_id);
  return *accent_phrases;
}

std::vector<AccentPhrase> TTSEngine::create_accent_phrases(std::string const &text, StyleId style_id)
{
  std::vector<AccentPhrase> accent_phrases = text_to_accent_phrases(text);
  accent_phrases = this->update_length_and_pitch(&accent_phrases, style_id);
  return accent_phrases;
}

std::vector<AccentPhrase> TTSEngine::create_accent_phrases_from_kana(std::string const &kana, StyleId style_id)
{
  std::vector<AccentPhrase> accent_phrases = parse_kana(kana);
  accent_phrases = this->update_length_and_pitch(&accent_phrases, style_id);
  return accent_phrases;
}

AudioFile<float> TTSEngine::synthesize_wave(AudioQuery *query, StyleId style_id, bool enable_interrogative_upspeak)
{
  AudioQuery query_copy = copy_audio_query(query);
  *query_copy.accent_phrases = apply_interrogative_upspeak(query_copy.accent_phrases, enable_interrogative_upspeak);
  auto [phoneme, f0] = query_to_decoder_feature(&query_copy);
  auto [raw_wave_like, sr_raw_wave] = this->core->safe_decode_forward(&phoneme, &f0, style_id);
  AudioFile<float> raw_wave;
  raw_wave.setAudioBuffer(raw_wave_like);
  AudioFile<float> wave = raw_wave_to_output_wave(&query_copy, &raw_wave, sr_raw_wave);
  return wave;
}

void TTSEngine::initialize_synthesis(StyleId style_id, bool skip_reinit)
{
  this->core->initialize_style_id_synthesis(style_id, skip_reinit);
}

bool TTSEngine::is_synthesis_initialized(StyleId style_id)
{
  return this->core->is_initialized_style_id_synthesis(style_id);
}

std::tuple<std::vector<FramePhoneme>, std::vector<float>, std::vector<float>> TTSEngine::create_sing_phoneme_and_f0_and_volume(Score *score, StyleId style_id)
{
  std::vector<Note> *notes = score->notes;
  auto [note_lengths_array,
        note_consonants_array,
        note_vowels_array,
        phonemes_array,
        phoneme_keys_array,
        phoneme_note_ids] = notes_to_keys_and_phonemes(notes);
  std::vector<int64_t> consonant_lengths = this->core->safe_predict_sing_consonant_length_forward(&note_consonants_array, &note_vowels_array, &note_lengths_array, style_id);
  std::vector<int64_t> phoneme_lengths = calc_phoneme_lengths(&consonant_lengths, &note_lengths_array);
  std::vector<int64_t> frame_phonemes = numpy_repeat(&phonemes_array, &phoneme_lengths);
  std::vector<int64_t> frame_keys = numpy_repeat(&phoneme_keys_array, &phoneme_lengths);
  std::vector<float> f0s = this->core->safe_predict_sing_f0_forward(&frame_phonemes, &frame_keys, style_id);
  std::vector<float> volumes = this->core->safe_predict_sing_volume_forward(&frame_phonemes, &frame_keys, &f0s, style_id);
  std::vector<FramePhoneme> phoneme_data_list;
  for (size_t i = 0; i < phonemes_array.size(); i++)
  {
    FramePhoneme frame_phoneme;
    frame_phoneme.phoneme = PhonemeList.at(phonemes_array[i]);
    frame_phoneme.frame_length = phoneme_lengths[i];
    frame_phoneme.note_id = phoneme_note_ids[i];
    phoneme_data_list.emplace_back(frame_phoneme);
  }
  return {phoneme_data_list, f0s, volumes};
}

std::vector<float> TTSEngine::create_sing_volume_from_phoneme_and_f0(Score *score, std::vector<FramePhoneme> *phonemes, std::vector<float> *f0s, StyleId style_id)
{
  std::vector<Note> *notes = score->notes;
  auto [_1, _2, _3, phonemes_array_from_notes, phoneme_keys_array, _4] = notes_to_keys_and_phonemes(notes);
  std::vector<int64_t> phonemes_array;
  std::vector<int64_t> phoneme_lengths;
  for (auto &&p : *phonemes)
  {
    phonemes_array.emplace_back(Phoneme::id(p.phoneme));
    phoneme_lengths.emplace_back(p.frame_length);
  }
  std::vector<float> f0_array = *f0s;

  for (size_t i = 0; i < phonemes_array.size(); i++)
  {
    if (phonemes_array[i] != phonemes_array_from_notes[i])
      throw "Scoreから抽出した音素列とFrameAudioQueryから抽出した音素列が一致しません。";
  }
  std::vector<int64_t> frame_phonemes = numpy_repeat(&phonemes_array, &phoneme_lengths);
  std::vector<int64_t> frame_keys = numpy_repeat(&phoneme_keys_array, &phoneme_lengths);
  std::vector<float> volumes = this->core->safe_predict_sing_volume_forward(&frame_phonemes, &frame_keys, &f0_array, style_id);
  return volumes;
}

AudioFile<float> TTSEngine::frame_synthsize_wave(FrameAudioQuery *query, StyleId style_id)
{
  auto [phoneme, f0, volume] = frame_query_to_sf_decoder_feature(query);
  auto [raw_wave_like, sr_raw_wave] = this->core->safe_sf_decode_forward(&phoneme, &f0, &volume, style_id);
  AudioFile<float> raw_wave;
  raw_wave.setAudioBuffer(raw_wave_like);
  AudioFile<float> wave = raw_wave_to_output_wave(query, &raw_wave, sr_raw_wave);
  return wave;
}

std::vector<std::string> TTSEngineManager::versions() const
{
  std::vector<std::string> versions;
  for (auto &&[k, v] : this->engines)
  {
    versions.emplace_back(std::string(k));
  }
  return versions;
}

std::string TTSEngineManager::latest_version()
{
  std::vector<std::string> versions = this->versions();
  return get_latest_version(&versions);
}

void TTSEngineManager::register_engine(TTSEngine &engine, std::string const &version)
{
  this->engines.emplace(version, engine);
}

TTSEngine TTSEngineManager::get_engine()
{
  return this->engines.at(this->latest_version());
}

TTSEngine TTSEngineManager::get_engine(std::string const &version)
{
  return this->engines.at(version);
}

TTSEngineManager make_tts_engines_from_cores(CoreManager *core_manager)
{
  const std::string MockVer = "0.0.0";
  TTSEngineManager tts_engines;
  for (auto &&[ver, core] : core_manager->items())
  {
    if (ver == MockVer)
      ;
    //   tts_engines.register_engine(MockTTSEngine(), ver);
    else
    {
      TTSEngine tts_engine;
      tts_engine.initialize(core.core);
      tts_engines.register_engine(tts_engine, ver);
    }
  }
  return tts_engines;
}
