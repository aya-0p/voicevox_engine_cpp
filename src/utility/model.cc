#include "./model.h"
#include "../tts_pipeline/model.h"

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
