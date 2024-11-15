#ifndef VOICEVOX_ENGINE_ENGINE_MANIFEST
#define VOICEVOX_ENGINE_ENGINE_MANIFEST
#include <string>
#include <vector>
#include <filesystem>

typedef struct FeatureSupportJson
{
  std::string type;
  bool value;
  std::string name;
} FeatureSupportJson;

typedef struct SupportedFeaturesJson
{
  FeatureSupportJson *adjust_mora_pitch = new FeatureSupportJson();
  FeatureSupportJson *adjust_phoneme_length = new FeatureSupportJson();
  FeatureSupportJson *adjust_speed_scale = new FeatureSupportJson();
  FeatureSupportJson *adjust_pitch_scale = new FeatureSupportJson();
  FeatureSupportJson *adjust_intonation_scale = new FeatureSupportJson();
  FeatureSupportJson *adjust_volume_scale = new FeatureSupportJson();
  FeatureSupportJson *adjust_pause_length = new FeatureSupportJson();
  FeatureSupportJson *interrogative_upspeak = new FeatureSupportJson();
  FeatureSupportJson *synthesis_morphing = new FeatureSupportJson();
  FeatureSupportJson *sing = new FeatureSupportJson();
  FeatureSupportJson *manage_library = new FeatureSupportJson();
  FeatureSupportJson *return_resource_url = new FeatureSupportJson();
} SupportedFeaturesJson;

typedef struct EngineManifestJson
{
  std::string manifest_version = "";
  std::string name = "";
  std::string brand_name = "";
  std::string uuid = "";
  std::string version = "";
  std::string url = "";
  std::string command = "";
  int32_t port = 0;
  std::string icon = "";
  int32_t default_sampling_rate = 0;
  double frame_rate = 0.0;
  std::string terms_of_service = "";
  std::string update_infos = "";
  std::string dependency_licenses = "";
  SupportedFeaturesJson *supported_features = new SupportedFeaturesJson();
} EngineManifestJson;

typedef struct UpdateInfo
{
  std::string version = "";
  std::vector<std::string> *description = new std::vector<std::string>();
  std::vector<std::string> *contributors = new std::vector<std::string>();
} UpdateInfo;

typedef struct LicenseInfo
{
  std::string name = "";
  std::string version = "";
  std::string license = "";
  std::string text;
} LicenseInfo;

typedef struct SupportedFeatures
{
  bool adjust_mora_pitch = false;
  bool adjust_phoneme_length = false;
  bool adjust_speed_scale = false;
  bool adjust_pitch_scale = false;
  bool adjust_intonation_scale = false;
  bool adjust_volume_scale = false;
  bool adjust_pause_length = false;
  bool interrogative_upspeak = false;
  bool synthesis_morphing = false;
  bool sing = false;
  bool manage_library = false;
  bool return_resource_url = false;
} SupportedFeatures;

#define EngineName std::string
#define BrandName std::string
typedef struct EngineManifest
{
  std::string manifest_version = "";
  EngineName name = "";
  BrandName brand_name = "";
  std::string uuid = "";
  std::string url = "";
  std::string icon = "";
  int32_t default_sampling_rate = 0;
  double frame_rate = 0;
  std::string terms_of_service = "";
  std::vector<UpdateInfo> *update_infos = new std::vector<UpdateInfo>();
  std::vector<LicenseInfo> *dependency_licenses = new std::vector<LicenseInfo>();
  std::string supported_vvlib_manifest_version = "";
  SupportedFeatures *supported_features = new SupportedFeatures();
} EngineManifest;
#undef EngineName
#undef EngineName

EngineManifest load_manifest(std::filesystem::path manifest_path);

#endif
