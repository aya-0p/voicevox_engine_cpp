#include "./phoneme.h"
#include <vector>
#include <string>
std::vector<std::string> BaseVowel = {"pau", "N", "a", "cl", "e", "i", "o", "u"};
std::vector<std::string> Vowel = {"pau", "N", "a", "cl", "e", "i", "o", "u", "A", "E", "I", "O", "U"};
std::vector<std::string> Consonant = {
    "b",
    "by",
    "ch",
    "d",
    "dy",
    "f",
    "g",
    "gw",
    "gy",
    "h",
    "hy",
    "j",
    "k",
    "kw",
    "ky",
    "m",
    "my",
    "n",
    "ny",
    "p",
    "py",
    "r",
    "ry",
    "s",
    "sh",
    "t",
    "ts",
    "ty",
    "v",
    "w",
    "y",
    "z",
};
std::vector<std::string> PhonemeList = {
    "pau", "A", "E", "I", "N", "O", "U", "a", "b", "by",
    "ch", "cl", "d", "dy", "e", "f", "g", "gw", "gy", "h",
    "hy", "i", "j", "k", "kw", "ky", "m", "my", "n", "ny",
    "o", "p", "py", "r", "ry", "s", "sh", "t", "ts", "ty",
    "u", "v", "w", "y", "z"};
std::vector<std::string> UnvoicedMoraTailPhonemes = {"A", "I", "U", "E", "O", "cl", "pau"};
std::vector<std::string> MoraTailPhonemes = {"A", "I", "U", "E", "O", "cl", "pau", "a", "i", "u", "e", "o", "N"};

void Phoneme::initialize(std::string const &phoneme)
{
  std::string phoneme_copy(phoneme);
  if (phoneme_copy == "sil")
    phoneme_copy = "pau";
  this->phoneme = phoneme_copy;
}

int32_t Phoneme::id() const
{
  for (int32_t i = 0; i < PhonemeList.size(); i++)
  {
    if (this->phoneme == PhonemeList[i])
      return i;
  }
  return -1;
}

std::vector<float> Phoneme::onehot() const
{
  return std::vector<float>(PhonemeList.size(), 1);
}

bool Phoneme::is_mora_tail() const
{
  for (auto &&phoneme : MoraTailPhonemes)
  {
    if (this->phoneme == phoneme)
      return true;
  }
  return false;
}

bool Phoneme::is_unvoiced_mora_tail() const
{
  for (auto &&phoneme : UnvoicedMoraTailPhonemes)
  {
    if (this->phoneme == phoneme)
      return true;
  }
  return false;
}

int32_t Phoneme::id(std::string phoneme)
{
  if (phoneme == "sil")
    phoneme = "pau";
  for (int32_t i = 0; i < PhonemeList.size(); i++)
  {
    if (phoneme == PhonemeList[i])
      return i;
  }
  return -1;
}
