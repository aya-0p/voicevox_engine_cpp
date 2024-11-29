#ifndef VOICEVOX_ENGINE_JSON_PARSER
#define VOICEVOX_ENGINE_JSON_PARSER
#include <string>
#include "./model.h"
using json = nlohmann::json;
std::string create_audio_query_json(AudioQuery *audio_query);
AudioQuery parse_audio_query_json(std::string const &audio_query_json_string);
#endif
