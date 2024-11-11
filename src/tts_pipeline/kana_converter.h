#ifndef VOICEVOX_ENGINE_KANA_CONVERTER
#define VOICEVOX_ENGINE_KANA_CONVERTER
#include <map>
#include <string>
#include "./model.h"

AccentPhrase text_to_accent_phrase(std::vector<std::string> *phrase);
std::vector<AccentPhrase> parse_kana(std::string const &text);
std::string create_kana(std::vector<AccentPhrase> *accent_phrases);
enum CharState
{
  None = 0,
  WaifForSecondChar = 1,
  SecondCharIs0x80 = 2,
  SecondCharIs0x82 = 3,
  SecondCharIs0x83 = 4,
};
#endif
