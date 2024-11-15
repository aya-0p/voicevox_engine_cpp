#include "./tts_pipeline/kana_converter.h"
#include "./app/application.h"
#include "./core/core_initializer.h"
#include "./tts_pipeline/tts_engine.h"
#include <iostream>
#include <map>

std::map<std::string, std::string> read_cli_arguments(int args, char *argv[])
{
  return std::map<std::string, std::string>();
}

int main(int args, char *argv[])
{
  init_kana_converter();
  std::vector<std::string> voicelib_dirs = {"/home/aya/voicevox/v0.15/"};
  std::vector<std::string> runtime_dirs = {"/home/aya/voicevox/v0.15/"};
  CoreManager core_manager = initialize_cores(false, &voicelib_dirs, "", &runtime_dirs, 0, false, true);
  TTSEngineManager tts_engines = make_tts_engines_from_cores(&core_manager);
  for (size_t i = 1; i < args; i++)
  {
    std::cout << argv[i] << std::endl;
  }
  serve(&tts_engines);
  
  return 0;
}
