#ifndef VOICEVOX_ENGINE_TEXT_ANALYZER
#define VOICEVOX_ENGINE_TEXT_ANALYZER
#include <vector>
#include <string>
#include <map>
#include "./model.h"
extern std::vector<std::string> OjtVowel;
extern std::vector<std::string> OjtConsonant;
extern std::vector<std::string> OjtUnknown;
extern std::vector<std::string> OjtPhoneme;
class Label
{
public:
  void initialize(std::map<std::string, std::string> contexts);
  static Label from_feature(std::string const &feature);
  std::string phoneme() const;
  int32_t mora_index() const;
  bool is_pause() const;
  int32_t accent_position() const;
  bool is_interrogative() const;
  std::string accent_phrase_index() const;
  std::string breath_group_index() const;
  bool is_valid() const;

private:
  std::map<std::string, std::string> contexts = std::map<std::string, std::string>();
};

class MoraLabel
{
public:
  void initialize(Label consonant, Label vowel);
  Label consonant = Label();
  Label vowel = Label();
  std::vector<Label> labels() const;
};

class AccentPhraseLabel
{
public:
  void initialize(std::vector<MoraLabel> moras, int32_t accent, bool is_interrogative);
  std::vector<MoraLabel> moras = std::vector<MoraLabel>();
  int32_t accent = 0;
  bool is_interrogative = false;
  static AccentPhraseLabel from_labels(std::vector<Label> const &labels);
  std::vector<Label> labels() const;
};

class BreathGroupLabel
{
public:
  void initialize(std::vector<AccentPhraseLabel> accent_phrases);
  std::vector<AccentPhraseLabel> accent_phrases = std::vector<AccentPhraseLabel>();
  static BreathGroupLabel from_labels(std::vector<Label> const &labels);
  std::vector<Label> labels() const;
};

class UtteranceLabel
{
public:
  void initialize(std::vector<BreathGroupLabel> breath_groups, std::vector<Label> pauses);
  std::vector<BreathGroupLabel> breath_groups = std::vector<BreathGroupLabel>();
  std::vector<Label> pauses = std::vector<Label>();
  static UtteranceLabel from_labels(std::vector<Label> const &labels);
  std::vector<Label> labels() const;
};

std::string mora_to_text(std::string const &mora_phonemes);
std::vector<Mora> mora_labels_to_moras(std::vector<MoraLabel> *mora_labels);
std::vector<AccentPhrase> utterance_to_accent_phrases(UtteranceLabel *utterance);
std::vector<AccentPhrase> text_to_accent_phrases(std::string const &text);

#endif
