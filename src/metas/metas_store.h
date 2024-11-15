#ifndef VOICEVOX_ENGINE_METAS_STORE
#define VOICEVOX_ENGINE_METAS_STORE
#include "../core/core_adapter.h"
#include "./metas.h"
#include <vector>
#include <string>
std::vector<SpeakerStyle> cast_styles(std::vector<CoreCharacterStyle>);
typedef struct Character
{
  std::string name = "";
  std::string uuid = "";
  std::vector<SpeakerStyle> *talk_styles = new std::vector<SpeakerStyle>();
  std::vector<SpeakerStyle> *sing_styles = new std::vector<SpeakerStyle>;
  std::string version = "";
  SpeakerSupportedFeatures *supported_features = new SpeakerSupportedFeatures();
} Character;

typedef struct EngineCharacter
{
  SpeakerSupportedFeatures *supported_features = new SpeakerSupportedFeatures();
} EngineCharacter;
#endif
