#include "./model.h"
#include "./tts_pipeline/model.h"

AudioQuery copy_audio_query(AudioQuery *audio_query)
{
  AudioQuery audio_query_new;
  audio_query_new.accent_phrases->reserve(audio_query->accent_phrases->size());
  for (auto &&accent_phrase : *(audio_query->accent_phrases))
  {
    AccentPhrase accent_phrase_copy = copy_accent_phrase(&accent_phrase);
    audio_query_new.accent_phrases->emplace_back(accent_phrase_copy);
  }
  audio_query_new.speedScale = audio_query->speedScale;
  audio_query_new.pitchScale = audio_query->pitchScale;
  audio_query_new.intonationScale = audio_query->intonationScale;
  audio_query_new.volumeScale = audio_query->volumeScale;
  audio_query_new.prePhonemeLength = audio_query->prePhonemeLength;
  audio_query_new.postPhonemeLength = audio_query->postPhonemeLength;
  audio_query_new.pauseLength = audio_query->pauseLength;
  audio_query_new.pauseLengthScale = audio_query->pauseLengthScale;
  audio_query_new.outputSamplingRate = audio_query->outputSamplingRate;
  audio_query_new.outputStereo = audio_query->outputStereo;
  audio_query_new.kana = std::string(audio_query->kana);
  return audio_query_new;
}

std::string create_mora_json(Mora *mora)
{
  std::string mora_text = "{";
  mora_text += "\"text\":\"";
  mora_text += mora->text;
  mora_text += "\"";
  if (mora->consonant != "" && mora->consonant_length != -1)
  {
    mora_text += ",\"consonant\":\"";
    mora_text += mora->consonant;
    mora_text += "\",\"consonant_length\":";
    mora_text += std::to_string(mora->consonant_length);
  }
  mora_text += ",\"vowel\":\"";
  mora_text += mora->vowel;
  mora_text += "\",\"vowel_length\":";
  mora_text += std::to_string(mora->vowel_length);
  mora_text += "\",\"pitch\":";
  mora_text += std::to_string(mora->pitch);
  mora_text += "}";
  return mora_text;
}

std::string create_accent_phrase_json(AccentPhrase *accent_phrase)
{
  std::string accent_phrase_text = "{";
  accent_phrase_text += "{";
  accent_phrase_text += "\"moras\":[";
  for (size_t i = 0; i < accent_phrase->moras->size(); i++)
  {
    if (i > 0) accent_phrase_text += ",";
    accent_phrase_text += create_mora_json(&(*accent_phrase->moras)[i]);
  }
  
  accent_phrase_text += "],\"accent\":";
  accent_phrase_text += std::to_string(accent_phrase->accent);
  if (accent_phrase->pause_mora != nullptr)
  {
    accent_phrase_text += ",\"pause_mora\":";
    accent_phrase_text += create_mora_json(accent_phrase->pause_mora);
  }
  accent_phrase_text += ",\"is_interrogative\":";
  accent_phrase_text += accent_phrase->is_interrogative ? "true" : "false";
  accent_phrase_text += "}";
  return accent_phrase_text;
}

std::string create_audio_query_json(AudioQuery *audio_query)
{
  std::string audio_query_text = "{";
  audio_query_text += "\"accent_phrases\":[";
  for (size_t i = 0; i < audio_query->accent_phrases->size(); i++)
  {
    if (i > 0) audio_query_text += ",";
    audio_query_text += create_accent_phrase_json(&(*audio_query->accent_phrases)[i]);
  }
  audio_query_text += "],\"speedScale\":";
  audio_query_text += std::to_string(audio_query->speedScale);
  audio_query_text += ",\"pitchScale\":";
  audio_query_text += std::to_string(audio_query->pitchScale);
  audio_query_text += ",\"intonationScale\":";
  audio_query_text += std::to_string(audio_query->intonationScale);
  audio_query_text += ",\"volumeScale\":";
  audio_query_text += std::to_string(audio_query->volumeScale);
  audio_query_text += ",\"prePhonemeLength\":";
  audio_query_text += std::to_string(audio_query->prePhonemeLength);
  audio_query_text += ",\"postPhonemeLength\":";
  audio_query_text += std::to_string(audio_query->postPhonemeLength);
  if (audio_query->pauseLength != -1)
  {
    audio_query_text += ",\"pauseLength\":";
    audio_query_text += std::to_string(audio_query->pauseLength);
  }
  audio_query_text += ",\"pauseLengthScale\":";
  audio_query_text += std::to_string(audio_query->pauseLengthScale);
  audio_query_text += ",\"outputSamplingRate\":";
  audio_query_text += std::to_string(audio_query->outputSamplingRate);
  audio_query_text += ",\"outputStereo\":";
  audio_query_text += audio_query->outputStereo ? "true" : "false";
  if (audio_query->kana != "")
  {
    audio_query_text += ",\"kana\":\"";
    audio_query_text += audio_query->kana;
    audio_query_text += "\"";
  }
  audio_query_text += "}";
  return audio_query_text;
}
