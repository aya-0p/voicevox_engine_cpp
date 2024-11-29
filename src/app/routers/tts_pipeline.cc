#include "./tts_pipeline.h"
#include "../../libraries/httplib.h"
#include "../../tts_pipeline/tts_engine.h"
#include "../../libraries/json.hpp"
#include "../../tts_pipeline/kana_converter.h"
#include "../../json_parser.h"
#include "../../utility/file_utility.h"

using json = nlohmann::json;

void router_tts_pipeline(httplib::Server *server, TTSEngineManager *tts_engines)
{
  server->Post("/audio_query", [tts_engines](const httplib::Request &req, httplib::Response &res)
               {
    std::string text = req.get_param_value("text");
    std::string version = req.has_param("core_version") ? req.get_param_value("core_version") : "";
    std::string style_id = req.get_param_value("style_id");
    if (style_id.empty())
    {
      res.set_content("", "text/plain");
    }
    else
    {
      auto engine = version.empty() ? tts_engines->get_engine() : tts_engines->get_engine(version);
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
    } });
  server->Post("/synthesis", [tts_engines](const httplib::Request &req, httplib::Response &res) {
    std::string version = req.has_param("core_version") ? req.get_param_value("core_version") : "";
    std::string style_id = req.get_param_value("style_id");
    bool enable_interrogative_upspeak = !req.has_param("enable_interrogative_upspeak") || req.get_param_value("enable_interrogative_upspeak") == "true";
    std::string audio_query_json = req.body;
    AudioQuery audio_query = parse_audio_query_json(audio_query_json);
    TTSEngine engine = version.empty() ? tts_engines->get_engine() : tts_engines->get_engine(version);
    AudioFile<float> wave = engine.synthesize_wave(&audio_query, std::stoi(style_id), enable_interrogative_upspeak);
    std::filesystem::path temp_path = std::filesystem::temp_directory_path() / "temp.wav";
    wave.save(temp_path, AudioFileFormat::Wave);
    res.set_file_content(temp_path, "audio/wav");
  });
  server->Post("/sing_frame_audio_query", [tts_engines](const httplib::Request &req, httplib::Response &res) {
    std::string version = req.has_param("core_version") ? req.get_param_value("core_version") : "";
    std::string style_id = req.get_param_value("style_id");
    if (style_id.empty())
    {
      res.set_content("", "text/plain");
    }
    else
    {
      std::string score_json = req.body;
      Score score = parse_score_json(score_json);
      TTSEngine engine = version.empty() ? tts_engines->get_engine() : tts_engines->get_engine(version);
      try
      {
        auto [phonemes, f0, volume] = engine.create_sing_phoneme_and_f0_and_volume(&score, std::stoi(style_id));
        FrameAudioQuery frame_audio_query;
        for (auto &&f0_ : f0)
        {
          frame_audio_query.f0->emplace_back(f0_);
        }
        for (auto &&vol : volume)
        {
          frame_audio_query.volume->emplace_back(vol);
        }
        for (auto &&ph : phonemes)
        {
          frame_audio_query.phonemes->emplace_back(ph);
        }
        frame_audio_query.volumeScale = 1;
        frame_audio_query.outputSamplingRate = engine.default_sampling_rate;
        frame_audio_query.outputStereo = false;
        std::string frame_audio_query_json = create_frame_audio_query_json(&frame_audio_query);
      res.set_content(frame_audio_query_json, "application/json");
      }
      catch(const std::string& str)
      {
        res.set_content(str, "text/plain");
      }
    }
  });
}
