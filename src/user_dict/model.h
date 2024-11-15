#ifndef VOICEVOX_ENGINE_USER_DICT_MODEL
#define VOICEVOX_ENGINE_USER_DICT_MODEL
#include <string>

#define USER_DICT_MIN_PRIORITY 0
#define USER_DICT_MAX_PRIORITY 10

typedef struct UserDictWord
{
  std::string surface = "";
  int32_t priority = 0;
  int32_t context_id = 0;
  std::string part_of_speech = "";
  std::string part_of_speech_detail_1 = "";
  std::string part_of_speech_detail_2 = "";
  std::string part_of_speech_detail_3 = "";
  std::string inflectional_type = "";
  std::string inflectional_form = "";
  std::string stem = "";
  std::string yomi = "";
  std::string pronunciation = "";
  int32_t accent_type = 0;
  int32_t mora_count = 0;
  std::string accent_associative_rule = "";
} UserDictWord;

std::string convert_to_zenkaku(std::string surface);

std::string check_is_katakana(std::string pronunciation);

UserDictWord check_mora_count_and_accent_type(UserDictWord *dict_word);

#endif
