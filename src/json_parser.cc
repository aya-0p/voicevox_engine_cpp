#include <string>
#include "./model.h"
#include "./libraries/json.hpp"
#include "./util.h"

using json = nlohmann::json;

std::string create_mora_json(Mora *mora)
{
  std::string mora_text = "{";
  mora_text += "\"text\":\"";
  mora_text += mora->text;
  mora_text += "\"";
  if (mora->consonant != "")
  {
    mora_text += ",\"consonant\":\"";
    mora_text += mora->consonant;
    mora_text += "\"";
  }
  else
  {
    mora_text += ",\"consonant\":null";
  }
  mora_text += ",\"consonant_length\":";
  if (mora->consonant_length != -1)
  {
    mora_text += to_string(mora->consonant_length);
  }
  else
  {
    mora_text += "null";
  }
  mora_text += ",\"vowel\":\"";
  mora_text += mora->vowel;
  mora_text += "\",\"vowel_length\":";
  mora_text += to_string(mora->vowel_length);
  mora_text += ",\"pitch\":";
  mora_text += to_string(mora->pitch);
  mora_text += "}";
  return mora_text;
}

std::string create_accent_phrase_json(AccentPhrase *accent_phrase)
{
  std::string accent_phrase_text = "{";
  accent_phrase_text += "\"moras\":[";
  for (size_t i = 0; i < accent_phrase->moras->size(); i++)
  {
    if (i > 0)
      accent_phrase_text += ",";
    accent_phrase_text += create_mora_json(&(*accent_phrase->moras)[i]);
  }

  accent_phrase_text += "],\"accent\":";
  accent_phrase_text += to_string(accent_phrase->accent);
  accent_phrase_text += ",\"pause_mora\":";
  if (accent_phrase->pause_mora != nullptr)
  {
    accent_phrase_text += create_mora_json(accent_phrase->pause_mora);
  }
  else
  {
    accent_phrase_text += "null";
  }
  accent_phrase_text += ",\"is_interrogative\":";
  accent_phrase_text += to_string(accent_phrase->is_interrogative);
  accent_phrase_text += "}";
  return accent_phrase_text;
}

std::string create_audio_query_json(AudioQuery *audio_query)
{
  std::string audio_query_text = "{";
  audio_query_text += "\"accent_phrases\":[";
  for (size_t i = 0; i < audio_query->accent_phrases->size(); i++)
  {
    if (i > 0)
      audio_query_text += ",";
    audio_query_text += create_accent_phrase_json(&(*audio_query->accent_phrases)[i]);
  }
  audio_query_text += "],\"speedScale\":";
  audio_query_text += to_string(audio_query->speedScale);
  audio_query_text += ",\"pitchScale\":";
  audio_query_text += to_string(audio_query->pitchScale);
  audio_query_text += ",\"intonationScale\":";
  audio_query_text += to_string(audio_query->intonationScale);
  audio_query_text += ",\"volumeScale\":";
  audio_query_text += to_string(audio_query->volumeScale);
  audio_query_text += ",\"prePhonemeLength\":";
  audio_query_text += to_string(audio_query->prePhonemeLength);
  audio_query_text += ",\"postPhonemeLength\":";
  audio_query_text += to_string(audio_query->postPhonemeLength);
  audio_query_text += ",\"pauseLength\":";
  if (audio_query->pauseLength != -1)
  {
    audio_query_text += to_string(audio_query->pauseLength);
  }
  else
  {
    audio_query_text += "null";
  }
  audio_query_text += ",\"pauseLengthScale\":";
  audio_query_text += to_string(audio_query->pauseLengthScale);
  audio_query_text += ",\"outputSamplingRate\":";
  audio_query_text += to_string(audio_query->outputSamplingRate);
  audio_query_text += ",\"outputStereo\":";
  audio_query_text += to_string(audio_query->outputStereo);
  if (audio_query->kana != "")
  {
    audio_query_text += ",\"kana\":\"";
    audio_query_text += audio_query->kana;
    audio_query_text += "\"";
  }
  else
  {
    audio_query_text += ",\"kana\":null";
  }
  audio_query_text += "}";
  return audio_query_text;
}

std::string create_frame_phoneme_json(FramePhoneme *frame_phoneme_json)
{
  std::string frame_phoneme = "{";
  frame_phoneme += "\"phoneme\":\"";
  frame_phoneme += frame_phoneme_json->phoneme;
  frame_phoneme += "\",\"frame_length\":";
  frame_phoneme += to_string(frame_phoneme_json->frame_length);
  frame_phoneme += ",\"note_id\":";
  if (frame_phoneme_json->note_id.empty())
  {
    frame_phoneme += "null";
  }
  else
  {
    frame_phoneme += "\"";
    frame_phoneme += frame_phoneme_json->note_id;
    frame_phoneme += "\"";
  }
  frame_phoneme += "}";
  return frame_phoneme;
}

std::string create_frame_audio_query_json(FrameAudioQuery *frame_audio_query)
{
  std::string frame_audio_query_json = "{";
  frame_audio_query_json += "\"f0\":[";
  for (size_t i = 0; i < frame_audio_query->f0->size(); i++)
  {
    if (i > 0) frame_audio_query_json += ",";
    frame_audio_query_json += to_string((*frame_audio_query->f0)[i]);
  }
  frame_audio_query_json += "],\"volume\":[";
  for (size_t i = 0; i < frame_audio_query->volume->size(); i++)
  {
    if (i > 0) frame_audio_query_json += ",";
    frame_audio_query_json += to_string((*frame_audio_query->volume)[i]);
  }
  frame_audio_query_json += "],\"phonemes\":[";
  for (size_t i = 0; i < frame_audio_query->phonemes->size(); i++)
  {
    if (i > 0) frame_audio_query_json += ",";
    frame_audio_query_json += create_frame_phoneme_json(&(*frame_audio_query->phonemes)[i]);
  }
  frame_audio_query_json += "],\"volumeScale\":";
  frame_audio_query_json += to_string(frame_audio_query->volumeScale);
  frame_audio_query_json += ",\"outputSamplingRate\":";
  frame_audio_query_json += to_string(frame_audio_query->outputSamplingRate);
  frame_audio_query_json += ",\"outputStereo\":";
  frame_audio_query_json += to_string(frame_audio_query->outputStereo);
  frame_audio_query_json += "}";
  return frame_audio_query_json;
}

int32_t get_int32_from_json(json &j, std::string const &key, bool allow_null = false)
{
  try
  {
    if (allow_null && (!j.contains(key) || j[key].is_null()))
      return -1;
    return j[key].get<int32_t>();
  }
  catch (const std::exception &e)
  {
    throw std::runtime_error("in json parser, key=" + key + ", required: number(int), got: " + j[key].type_name());
  }
}

float get_float_from_json(json &j, std::string const &key, bool allow_null = false)
{
  try
  {
    if (allow_null && (!j.contains(key) || j[key].is_null()))
      return -1;
    return j[key].get<float>();
  }
  catch (const std::exception &e)
  {
    throw std::runtime_error("in json parser, key=" + key + ", required: number(float), got: " + j[key].type_name());
  }
}

std::string get_string_from_json(json &j, std::string const &key, bool allow_null = false)
{
  if (allow_null && j[key].is_null())
    return "";
  if (j[key].type() == json::value_t::string)
    return j[key].get<std::string>();
  else
    throw std::runtime_error("in json parser, key=" + key + ", required: string, got: " + j[key].type_name());
}

bool get_bool_from_json(json &j, std::string const &key, bool allow_null = false)
{
  try
  {
    if (allow_null && j[key].is_null())
      return false;
    return j[key].get<bool>();
  }
  catch (const std::exception &e)
  {
    throw std::runtime_error("in json parser, key=" + key + ", required: bool, got: " + j[key].type_name());
  }
}

Mora parse_mora(json &mora_json)
{
  Mora mora;
  mora.text = get_string_from_json(mora_json, "text");
  mora.consonant = get_string_from_json(mora_json, "consonant", true);
  mora.consonant_length = get_float_from_json(mora_json, "consonant_length", true);
  mora.vowel = get_string_from_json(mora_json, "vowel");
  mora.vowel_length = get_float_from_json(mora_json, "vowel_length");
  mora.pitch = get_float_from_json(mora_json, "pitch");
  return mora;
}

AccentPhrase parse_accent_phrase(json &accent_phrase_json)
{
  AccentPhrase accent_phrase;
  for (auto &&mora_json : accent_phrase_json["moras"])
  {
    accent_phrase.moras->emplace_back(parse_mora(mora_json));
  }
  accent_phrase.accent = get_int32_from_json(accent_phrase_json, "accent");
  {
    json mora_json = accent_phrase_json["pause_mora"];
    if (mora_json.is_object())
    {
      accent_phrase.pause_mora->text = get_string_from_json(mora_json, "text");
      accent_phrase.pause_mora->consonant = get_string_from_json(mora_json, "consonant", true);
      accent_phrase.pause_mora->consonant_length = get_float_from_json(mora_json, "consonant_length", true);
      accent_phrase.pause_mora->vowel = get_string_from_json(mora_json, "vowel");
      accent_phrase.pause_mora->vowel_length = get_float_from_json(mora_json, "vowel_length");
      accent_phrase.pause_mora->pitch = get_float_from_json(mora_json, "pitch");
    }
    else accent_phrase.pause_mora = nullptr;
  }
  accent_phrase.is_interrogative = get_bool_from_json(accent_phrase_json, "is_interrogative");
  return accent_phrase;
}

AudioQuery parse_audio_query(json &audio_query_json)
{
  AudioQuery audio_query;
  for (auto &&accent_phrase_json : audio_query_json["accent_phrases"])
  {
    AccentPhrase accent_phrase = parse_accent_phrase(accent_phrase_json);
    audio_query.accent_phrases->emplace_back(accent_phrase);
  }
  audio_query.speedScale = get_float_from_json(audio_query_json, "speedScale");
  audio_query.pitchScale = get_float_from_json(audio_query_json, "pitchScale");
  audio_query.intonationScale = get_float_from_json(audio_query_json, "intonationScale");
  audio_query.volumeScale = get_float_from_json(audio_query_json, "volumeScale");
  audio_query.prePhonemeLength = get_float_from_json(audio_query_json, "prePhonemeLength");
  audio_query.postPhonemeLength = get_float_from_json(audio_query_json, "postPhonemeLength");
  audio_query.pauseLength = get_float_from_json(audio_query_json, "pauseLength", true);
  audio_query.pauseLengthScale = get_float_from_json(audio_query_json, "pauseLengthScale");
  audio_query.outputSamplingRate = get_int32_from_json(audio_query_json, "outputSamplingRate");
  audio_query.outputStereo = get_bool_from_json(audio_query_json, "outputStereo");
  audio_query.kana = get_string_from_json(audio_query_json, "kana", true);
  return audio_query;
}

AudioQuery parse_audio_query_json(std::string const &audio_query_json_string)
{
  json j = json::parse(audio_query_json_string);
  return parse_audio_query(j);
}

Note parse_note(json note_json)
{
  Note note;
  note.id = get_string_from_json(note_json, "id", true);
  note.key = get_int32_from_json(note_json, "key", true);
  note.frame_length = get_int32_from_json(note_json, "frame_length");
  note.lyric = get_string_from_json(note_json, "lyric");
  return note;
}

Score parse_score(json score_json)
{
  Score score;
  for (auto &&note_json : score_json["notes"])
  {
    Note note = parse_note(note_json);
    score.notes->emplace_back(note);
  }
  return score;
}

Score parse_score_json(std::string const &score_json)
{
  json j = json::parse(score_json);
  return parse_score(j);
}
