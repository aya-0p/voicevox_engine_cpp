#ifndef VOICEVOX_ENGINE_MODEL
#define VOICEVOX_ENGINE_MODEL
#include <vector>
#include <string>
#include "./tts_pipeline/model.h"
#include <cstdint>

typedef struct AudioQuery
{
  std::vector<AccentPhrase> *accent_phrases = new std::vector<AccentPhrase>();
  float speedScale = 0.0;
  float pitchScale = 0.0;
  float intonationScale = 0.0;
  float volumeScale = 0.0;
  float prePhonemeLength = 0.0;
  float postPhonemeLength = 0.0;
  // null? -> -1
  float pauseLength = -1;
  float pauseLengthScale = 1;
  int32_t outputSamplingRate = 0;
  bool outputStereo = false;
  // null? -> kana.empty()
  std::string kana = "";
} AudioQuery;
AudioQuery copy_audio_query(AudioQuery *audio_query);
#endif
