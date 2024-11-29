cmake_minimum_required(VERSION 3.10)
project(voicevox_engine_cpp VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
include(CheckIncludeFiles)
include(CheckFunctionExists)
check_include_files(fcntl.h HAVE_FCNTL_H)
check_include_files(sys/mman.h HAVE_SYS_MMAN_H)
check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(windows.h HAVE_WINDOWS_H)
check_function_exists(mmap HAVE_MMAP)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/open_jtalk/mecab/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/open_jtalk/mecab/config.h)
add_definitions(-DDIC_VERSION=102
                -DPACKAGE="open_jtalk"
                -DVERSION="${OPEN_JTALK_VERSION}")

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
  src/open_jtalk/jpcommon/jpcommon_node.cc
  src/open_jtalk/jpcommon/jpcommon_label.cc
  src/open_jtalk/jpcommon/jpcommon.cc
  src/open_jtalk/mecab/writer.cc
  src/open_jtalk/mecab/param.cc
  src/open_jtalk/mecab/context_id.cc
  src/open_jtalk/mecab/dictionary_rewriter.cc
  src/open_jtalk/mecab/feature_index.cc
  src/open_jtalk/mecab/viterbi.cc
  src/open_jtalk/mecab/utils.cc
  src/open_jtalk/mecab/dictionary.cc
  src/open_jtalk/mecab/lbfgs.cc
  src/open_jtalk/mecab/string_buffer.cc
  src/open_jtalk/mecab/connector.cc
  src/open_jtalk/mecab/nbest_generator.cc
  src/open_jtalk/mecab/char_property.cc
  src/open_jtalk/mecab/tagger.cc
  src/open_jtalk/mecab/mecab.cc
  src/open_jtalk/mecab/learner_tagger.cc
  src/open_jtalk/mecab/tokenizer.cc
  src/open_jtalk/njd/njd.cc
  src/open_jtalk/njd/njd_node.cc
  src/open_jtalk/text2mecab/text2mecab.cc
  src/open_jtalk/njd_set_accent_phrase/njd_set_accent_phrase.cc
  src/open_jtalk/njd_set_accent_type/njd_set_accent_type.cc
  src/open_jtalk/njd_set_digit/njd_set_digit.cc
  src/open_jtalk/njd_set_long_vowel/njd_set_long_vowel.cc
  src/open_jtalk/njd_set_pronunciation/njd_set_pronunciation.cc
  src/open_jtalk/njd_set_unvoiced_vowel/njd_set_unvoiced_vowel.cc
  src/open_jtalk/njd2jpcommon/njd2jpcommon.cc
  src/open_jtalk/mecab2njd/mecab2njd.cc
  src/open_jtalk/open_jtalk.cc
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
  src/json_parser.cc
  src/main.cc
  src/model.cc
  # src/resource_manager.cc
  src/util.cc
)
