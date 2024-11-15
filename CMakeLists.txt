cmake_minimum_required(VERSION 3.10)
project(voicevox_engine_cpp VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

add_executable(
  voicevox_engine_cpp
  src/app/routers/character.cc
  # src/app/routers/engine_info.cc
  # src/app/routers/library.cc
  # src/app/routers/morphing.cc
  # src/app/routers/portal_page.cc
  # src/app/routers/preset.cc
  # src/app/routers/setting.cc
  src/app/routers/tts_pipeline.cc
  # src/app/routers/user_dict.cc
  src/app/application.cc
  src/core/core_adapter.cc
  src/core/core_initializer.cc
  src/core/core_wrapper.cc
  # src/dev/core/mock.cc
  # src/dev/tts_engine/mock.cc
  # src/library/library_manager.cc
  # src/library/model.cc
  src/metas/metas_store.cc
  src/metas/metas.cc
  # src/morphing/model.cc
  # src/morphing/morphing.cc
  src/open_jtalk/main.cc
  # src/preset/model.cc
  # src/preset/preset_manager.cc
  # src/setting/model.cc
  # src/setting/setting_manager.cc
  src/tts_pipeline/connect_base64_waves.cc
  src/tts_pipeline/kana_converter.cc
  src/tts_pipeline/model.cc
  src/tts_pipeline/mora_mapping.cc
  src/tts_pipeline/phoneme.cc
  src/tts_pipeline/text_analyzer.cc
  src/tts_pipeline/tts_engine.cc
  # src/user_dict/model.cc
  # src/user_dict/user_dict_manager.cc
  # src/user_dict/user_dict_word.cc
  src/utility/core_version_utility.cc
  src/utility/file_utility.cc
  src/utility/path_utility.cc
  src/utility/runtime_utility.cc
  # src/cancellable_engine.cc
  # src/engine_manifest.cc
  src/main.cc
  src/model.cc
  # src/resource_manager.cc
  src/util.cc
)
