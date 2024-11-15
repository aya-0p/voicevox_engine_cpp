#ifndef VOICEVOX_ENGINE_METAS
#define VOICEVOX_ENGINE_METAS
#include <cstdint>
#include <string>
#include <vector>
#define StyleId int64_t
#define NoteId std::string
#define WaveLike std::vector<std::vector<float>>
#define StyleType std::string // talk, singing_teacher, frame_decode or sing

typedef struct SpeakerStyle
{
  std::string name = "";
  StyleId id = -1;
  StyleType type = "";
} SpeakerStyle;

typedef struct SpeakerSupportedFeatures
{
  std::string permitted_synthesis_morphing = "";
} SpeakerSupportedFeatures;

typedef struct Speaker
{
  std::string name = "";
  std::string speaker_uuid = "";
  std::vector<SpeakerStyle> *styles = new std::vector<SpeakerStyle>();
  std::string version;
  SpeakerSupportedFeatures *supported_features = new SpeakerSupportedFeatures();
} Speaker;

typedef struct StyleInfo
{
  StyleId id = -1;
  std::string icon = "";
  std::string portrait = "";
  std::vector<std::string> *voice_samples = new std::vector<std::string>();
} StyleInfo;

typedef struct SpeakerInfo
{
  std::string policy = "";
  std::string portrait = "";
  std::vector<StyleInfo> *style_infos = new std::vector<StyleInfo>();
} SpeakerInfo;

#endif
