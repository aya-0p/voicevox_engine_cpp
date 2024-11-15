#include "./tts_pipeline.h"
#include "../../libraries/httplib.h"
#include "../../tts_pipeline/tts_engine.h"
#include "../../libraries/json.hpp"
#include "../../tts_pipeline/kana_converter.h"

using json = nlohmann::json;

void router_tts_pipeline(httplib::Server *server, TTSEngineManager *tts_engines)
{
  server->Post("/audio_query", [tts_engines](const httplib::Request &req, httplib::Response &res) {
    std::string text = req.get_param_value("text");
    std::string version = req.has_param("core_version") ? req.get_param_value("core_version") : "";
    std::string style_id = req.get_param_value("style_id");
    if (style_id == "")
    {
      res.set_content("", "text/plain");
    }
    else
    {
      auto engine = version == "" ? tts_engines->get_engine() : tts_engines->get_engine(version);
      auto accent_phrases = engine.create_accent_phrases(text, std::stoi(style_id));
      AudioQuery audio_query;
      for (auto &&accent_phrase : accent_phrases)
      {
        audio_query.accent_phrases->emplace_back(accent_phrase);
      }
      audio_query.speedScale = 1;
      audio_query.pitchScale = 0;
      audio_query.intonationScale = 1;
      audio_query.volumeScale = 1;
      audio_query.prePhonemeLength = 0.1;
      audio_query.postPhonemeLength = 0.1;
      audio_query.pauseLength = -1;
      audio_query.pauseLengthScale = 1;
      audio_query.outputSamplingRate = engine.default_sampling_rate;
      audio_query.outputStereo = false;
      audio_query.kana = create_kana(&accent_phrases);
      std::string audio_query_json = create_audio_query_json(&audio_query);
      res.set_content(audio_query_json, "application/json");
    }
  });
}
