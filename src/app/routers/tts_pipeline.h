#ifndef VOICEVOX_ENGINE_ROUTER_TTS_PIPELINE
#define VOICEVOX_ENGINE_ROUTER_TTS_PIPELINE
#include "../../tts_pipeline/tts_engine.h"
#include "../../libraries/httplib.h"
void router_tts_pipeline(httplib::Server *server, TTSEngineManager *tts_engines);
#endif
