#ifndef VOICEVOX_ENGINE_MODEL
#define VOICEVOX_ENGINE_
#include <vector>
#include <string>
#include "./tts_pipeline/model.h"
#include <cstdint>

typedef struct AudioQuery
{
  std::vector<AccentPhrase> *accent_phrases = new std::vector<AccentPhrase>();
  float speedScale;
  float pitchScale;
  float intonationScale;
  float volumeScale;
  float prePhonemeLength;
  float postPhonemeLength;
  // null? -> -1
  float pauseLength = -1;
  float pauseLengthScale = 1;
  int32_t outputSamplingRate;
  bool outputStereo;
  // null? -> kana.empty()
  std::string kana;
} AudioQuery;
AudioQuery copy_audio_query(AudioQuery *audio_query);
std::string create_audio_query_json(AudioQuery *audio_query);
#endif
