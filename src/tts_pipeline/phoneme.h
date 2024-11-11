#ifndef VOICEVOX_ENGINE_PHONEME
#define VOICEVOX_ENGINE_PHONEME
#include <vector>
#include <string>
extern std::vector<std::string> BaseVowel;
extern std::vector<std::string> Vowel;
extern std::vector<std::string> Consonant;
extern std::vector<std::string> PhonemeList;
extern std::vector<std::string> UnvoicedMoraTailPhonemes;
extern std::vector<std::string> MoraTailPhonemes;

class Phoneme
{
public:
  void initialize(std::string const &phoneme);
  int32_t id() const;
  std::vector<float> onehot() const;
  bool is_mora_tail() const;
  bool is_unvoiced_mora_tail() const;

  static int32_t id(std::string phoneme);
private:
  std::string phoneme = "";
};
#endif
