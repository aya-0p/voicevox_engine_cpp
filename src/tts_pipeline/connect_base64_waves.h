#ifndef VOICEVOX_ENGINE_CONNECT_BASE64_WAVES
#define VOICEVOX_ENGINE_CONNECT_BASE64_WAVES
#include <vector>
#include <cstdint>
#include <string>
#include "../libraries/AudioFile.h"
std::vector<AudioFile<double>> decode_base64_waves(std::vector<std::string> *waves);
std::pair<std::vector<std::vector<double>>, int32_t> connect_base64_waves(std::vector<std::string> *waves);
#endif
