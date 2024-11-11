cmake_minimum_required(VERSION 3.10)
project(voicevox_engine_cpp VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

add_executable(
  voicevox_engine_cpp
  src/core/core_adapter.cc
  src/core/core_initializer.cc
  src/core/core_wrapper.cc
  src/tts_pipeline/connect_base64_waves.cc
  src/tts_pipeline/kana_converter.cc
  src/tts_pipeline/model.cc
  src/tts_pipeline/mora_mapping.cc
  src/tts_pipeline/phoneme.cc
  src/tts_pipeline/text_analyzer.cc
  src/tts_pipeline/tts_engine.cc
  src/utility/core_version_utility.cc
  src/utility/file_utility.cc
  src/utility/model.cc
  src/utility/path_utility.cc
  src/utility/runtime_utility.cc
  src/server.cc
  src/util.cc
)
