#include <string>
#include <cstdint>
#include <vector>
#include "./model.h"
#include "./kana_converter.h"
#include "./mora_mapping.h"
#include "iostream"

int32_t LoopLimit = 300;

std::string UnvoiceSymbol = "_";
std::string AccentSymbol = "'";
std::string NopauseDelimiter = "/";
std::string PauseDelimiter = "、";
std::string WideInterrogationMark = "？";

// 利用される文字は
// 0x27,0x2f,0x5f,
// {0xe3,0x80,0x81},{0xe3,0x80,0x82},
// {0xe3,0x82,0xa1}~{0xe3,0x82,0xbf},
// {0xe3,0x83,0x80}~{0xe3,0x83,0xba}

std::vector<std::string> split_string_kana(std::string const &text)
{
  size_t size = text.size();
  std::vector<std::string> data;
  CharState state = CharState::None;
  for (size_t i = 0; i < size; i++)
  {
    uint8_t ch = static_cast<uint8_t>(text[i]);
    switch (state)
    {
    case CharState::None:
      if (ch == 0 || ch == 32)
        continue;
      if (ch == 39)
        data.emplace_back("'");
      else if (ch == 47)
        data.emplace_back("/");
      else if (ch == 95)
        data.emplace_back("_");
      else if (ch == 0xe3)
        state = CharState::WaifForSecondChar;
      else
        throw "ParseError";
    case CharState::WaifForSecondChar:
      if (ch == 0x80)
        state = CharState::SecondCharIs0x80;
      else if (ch == 0x82)
        state = CharState::SecondCharIs0x82;
      else if (ch == 0x83)
        state = CharState::SecondCharIs0x83;
      else
        throw "ParseError";
    case CharState::SecondCharIs0x80:
      if (ch == 0x81)
      {
        data.emplace_back("、");
        state = CharState::None;
      }
      else if (ch == 0x82)
      {
        data.emplace_back("。");
        state = CharState::None;
      }
      else
        throw "ParseError";
    case CharState::SecondCharIs0x82:
      if (0xa1 <= ch && ch <= 0xbf)
      {
        data.emplace_back(std::string({(int8_t)0xe3, (int8_t)0x82, (int8_t)ch}));
        state = CharState::None;
      }
      else
        throw "ParseError";
    case CharState::SecondCharIs0x83:
      if (0x80 <= ch && ch <= 0xba)
      {
        data.emplace_back(std::string({(int8_t)0xe3, (int8_t)0x83, (int8_t)ch}));
        state = CharState::None;
      }
      else
        throw "ParseError";
    }
  }
  return data;
}

std::map<std::string, Mora> kana2mora = std::map<std::string, Mora>();
void init_kana_converter()
{
  init_mora_mapping();
  if (kana2mora.size() == 0)
  {
    std::cout << mora_kana_to_mora_phonemes.size() << std::endl;
    for (auto &&[kana, value] : mora_kana_to_mora_phonemes)
    {
      auto [consonant, vowel] = value;
      kana2mora.emplace(kana, Mora{kana, consonant, static_cast<float>(consonant.empty() ? -1 : 0), vowel, 0, 0});
      if (vowel == "a")
        kana2mora.emplace(UnvoiceSymbol + kana, Mora{kana, consonant, static_cast<float>(consonant.empty() ? -1 : 0), "A", 0, 0});
      if (vowel == "i")
        kana2mora.emplace(UnvoiceSymbol + kana, Mora{kana, consonant, static_cast<float>(consonant.empty() ? -1 : 0), "I", 0, 0});
      if (vowel == "u")
        kana2mora.emplace(UnvoiceSymbol + kana, Mora{kana, consonant, static_cast<float>(consonant.empty() ? -1 : 0), "U", 0, 0});
      if (vowel == "e")
        kana2mora.emplace(UnvoiceSymbol + kana, Mora{kana, consonant, static_cast<float>(consonant.empty() ? -1 : 0), "E", 0, 0});
      if (vowel == "o")
        kana2mora.emplace(UnvoiceSymbol + kana, Mora{kana, consonant, static_cast<float>(consonant.empty() ? -1 : 0), "O", 0, 0});
    }
  }
}

AccentPhrase text_to_accent_phrase(std::vector<std::string> *phrase)
{
  // -1 if this is none
  int accent_index = -1;
  std::vector<Mora> moras;

  uint32_t base_index = 0;
  std::string stack = "";
  std::string matched_text;
  uint32_t outer_loop = 0;

  while (base_index < phrase->size())
  {
    outer_loop++;
    if ((*phrase)[base_index] == AccentSymbol)
    {
      if (moras.size() == 0)
        throw "句頭にアクセントは置けません";
      if (accent_index != -1)
        throw "1つのアクセント句に二つ以上のアクセントは置けません";
      accent_index = moras.size();
      base_index++;
      continue;
    }
    for (size_t watch_index = base_index; watch_index < phrase->size(); watch_index++)
    {
      if ((*phrase)[watch_index] == AccentSymbol)
        break;
      stack += (*phrase)[watch_index];
      if (kana2mora.count(stack) > 0)
        matched_text = stack;
    }
    if (matched_text.empty())
    {
      throw "判別できない読み仮名があります";
    }
    else
    {
      moras.emplace_back(Mora(kana2mora.at(matched_text)));
      base_index += matched_text.size();
      stack = "";
      matched_text = "";
    }
    if (outer_loop > LoopLimit)
      throw "処理時に無限ループになってしまいました...バグ報告をお願いします。";
  }
  if (accent_index == -1)
    throw "アクセントを指定していないアクセント句があります";
  AccentPhrase accent_phrase;
  for (size_t i = 0; i < moras.size(); i++)
  {
    accent_phrase.moras->emplace_back(moras[i]);
  }
  accent_phrase.accent = accent_index;
  
  return accent_phrase;
}

std::vector<AccentPhrase> parse_kana(std::string const &text)
{
  std::vector<AccentPhrase> parsed_results;
  uint32_t phrase_base = 0;
  std::vector<std::string> fix_text = split_string_kana(text);
  for (size_t i = 0; i < text.size() + 1; i++)
  {
    if (text.size() == i || fix_text[i] == PauseDelimiter || fix_text[i] == NopauseDelimiter)
    {
      std::vector<std::string> phrase;
      bool is_interrogative = false;
      for (size_t j = phrase_base; j < i; j++)
      {
        phrase.emplace_back(fix_text[j]);
        if (fix_text[j] == WideInterrogationMark)
        {
          if (j == i - 1)
            is_interrogative = true;
          else
            throw "アクセント句末以外に「？」は置けません";
        }
      }
      phrase_base = i + 1;
      if (is_interrogative)
      {
        phrase.pop_back();
      }
      AccentPhrase accent_phrase = text_to_accent_phrase(&phrase);
      if (i < fix_text.size() && fix_text[i] == PauseDelimiter)
      {
        // 要検証
        accent_phrase.pause_mora = new Mora();
        accent_phrase.pause_mora->text = "、";
        accent_phrase.pause_mora->consonant = "";
        accent_phrase.pause_mora->consonant_length = -1;
        accent_phrase.pause_mora->vowel = "pau";
        accent_phrase.pause_mora->vowel_length = 0;
        accent_phrase.pause_mora->pitch = 0;
      }
      accent_phrase.is_interrogative = is_interrogative;
      parsed_results.emplace_back(accent_phrase);
    }
  }
  return parsed_results;
}

std::string create_kana(std::vector<AccentPhrase> *accent_phrases)
{
  std::string text;
  for (size_t i = 0; i < accent_phrases->size(); i++)
  {
    AccentPhrase phrase = (*accent_phrases)[i];
    for (size_t j = 0; j < phrase.moras->size(); j++)
    {
      Mora mora = (*phrase.moras)[j];
      if (mora.vowel == "A" || mora.vowel == "I" || mora.vowel == "U" || mora.vowel == "E" || mora.vowel == "O")
      {
        text += UnvoiceSymbol;
      }
      text += mora.text;
      if (phrase.accent == j + 1)
      {
        text += AccentSymbol;
      }
    }
    if (phrase.is_interrogative)
    {
      text += WideInterrogationMark;
    }
    if (i < accent_phrases->size() - 1)
    {
      if (phrase.pause_mora->vowel_length == -1)
      {
        text += NopauseDelimiter;
      }
      else
      {
        text += PauseDelimiter;
      }
    }
  }
  return text;
}
