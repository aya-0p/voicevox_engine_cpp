#ifndef VOICEVOX_ENGINE_JSON_PARSER
#define VOICEVOX_ENGINE_JSON_PARSER
#include <string>
#include "./model.h"
using json = nlohmann::json;
std::string create_audio_query_json(AudioQuery *audio_query);
std::string create_frame_audio_query_json(FrameAudioQuery *frame_audio_query);
AudioQuery parse_audio_query_json(std::string const &audio_query_json_string);
Score parse_score_json(std::string const &score_json);
#endif
