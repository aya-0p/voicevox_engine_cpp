#include <vector>
#include <string>
#include <map>
#include <regex>
#include "./model.h"
#include "./text_analyzer.h"
#include "./mora_mapping.h"

std::vector<std::string> OjtVowel = {"A", "E", "I", "N", "O", "U", "a", "cl", "e", "i", "o", "pau", "sil", "u"};
std::vector<std::string> OjtConsonant = {
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
std::vector<std::string> OjtUnknown = {"xx"};
std::vector<std::string> OjtPhoneme = {"A",
                                       "E",
                                       "I",
                                       "N",
                                       "O",
                                       "U",
                                       "a",
                                       "cl",
                                       "e",
                                       "i",
                                       "o",
                                       "pau",
                                       "sil",
                                       "u",
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
                                       "xx"};

std::vector<std::string> dict = {
    "p1", "p2", "p3", "p4", "p5",
    "a1", "a2", "a3",
    "b1", "b2", "b3",
    "c1", "c2", "c3",
    "d1", "d2", "d3",
    "e1", "e2", "e3", "e4", "e5",
    "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8",
    "g1", "g2", "g3", "g4", "g5",
    "h1", "h2",
    "i1", "i2", "i3", "i4", "i5", "i6", "i7", "i8",
    "j1", "j2",
    "k1", "k2", "k3"};

void Label::initialize(std::map<std::string, std::string> contexts)
{
  this->contexts = contexts;
}

Label Label::from_feature(std::string const &feature)
{
  std::smatch result;
  std::regex re(
      R"(^(.+?)\^(.+?)\-(.+?)\+(.+?)\=(.+?))"
      R"(/A\:(.+?)\+(.+?)\+(.+?))"
      R"(/B\:(.+?)\-(.+?)\_(.+?))"
      R"(/C\:(.+?)\_(.+?)\+(.+?))"
      R"(/D\:(.+?)\+(.+?)\_(.+?))"
      R"(/E\:(.+?)\_(.+?)\!(.+?)\_(.+?)\-(.+?))"
      R"(/F\:(.+?)\_(.+?)\#(.+?)\_(.+?)\@(.+?)\_(.+?)\|(.+?)\_(.+?))"
      R"(/G\:(.+?)\_(.+?)\%(.+?)\_(.+?)\_(.+?))"
      R"(/H\:(.+?)\_(.+?))"
      R"(/I\:(.+?)\-(.+?)\@(.+?)\+(.+?)\&(.+?)\-(.+?)\|(.+?)\+(.+?))"
      R"(/J\:(.+?)\_(.+?))"
      R"(/K\:(.+?)\+(.+?)\-(.+?)$)");
  if (std::regex_match(feature, result, re))
  {
    std::map<std::string, std::string> contexts;
    for (size_t i = 0; i < result.size(); i++)
    {
      if (i == 0)
        continue;
      auto rs = result[i];
      contexts.emplace(dict[i - 1], rs.str());
    }
    Label lab;
    lab.initialize(contexts);
    return lab;
  }
  else
  {
    throw "Regex not match";
  }
}

std::string Label::phoneme() const
{
  return std::string(this->contexts.at("p3"));
}

int32_t Label::mora_index() const
{
  return std::stoi(this->contexts.at("a2"));
}

bool Label::is_pause() const
{
  return this->contexts.at("f1") == "xx";
}

int32_t Label::accent_position() const
{
  return std::stoi(this->contexts.at("f2"));
}

bool Label::is_interrogative() const
{
  return this->contexts.at("f3") == "1";
}

std::string Label::accent_phrase_index() const
{
  return std::string(this->contexts.at("f5"));
}

std::string Label::breath_group_index() const
{
  return std::string(this->contexts.at("i3"));
}

bool Label::is_valid() const
{
  return this->contexts.size() != 0;
}

void MoraLabel::initialize(Label consonant, Label vowel)
{
  this->consonant = consonant;
  this->vowel = vowel;
}

std::vector<Label> MoraLabel::labels() const
{
  std::vector<Label> val;
  if (this->consonant.is_valid())
  {
    val.emplace_back(Label(this->consonant));
    val.emplace_back(Label(this->vowel));
  }
  else
  {
    val.emplace_back(Label(this->vowel));
  };
  return val;
}

void AccentPhraseLabel::initialize(std::vector<MoraLabel> moras, int32_t accent, bool is_interrogative)
{
  this->moras = moras;
  this->accent = accent;
  this->is_interrogative = is_interrogative;
}

AccentPhraseLabel AccentPhraseLabel::from_labels(std::vector<Label> const &labels)
{
  std::vector<MoraLabel> moras;
  std::vector<Label> mora_labels;
  for (size_t i = 0; i < labels.size(); i++)
  {
    Label label = labels[i];
    Label next_label = i == labels.size() - 1 ? Label() : labels[i + 1];
    if (label.mora_index() == 49)
      break;
    mora_labels.emplace_back(label);
    if (!next_label.is_valid() || label.mora_index() != next_label.mora_index())
    {
      Label consonant = mora_labels.size() == 1 ? Label() : mora_labels.size() == 2 ? mora_labels[0]
                                                                                    : throw "Err: mora_labels error";
      Label vowel = mora_labels.size() == 1 ? mora_labels[0] : mora_labels.size() == 2 ? mora_labels[1]
                                                                                       : throw "Err: mora_labels error";
      MoraLabel mora;
      mora.initialize(consonant, vowel);
      moras.emplace_back(mora);
      mora_labels.clear();
    }
  }
  int32_t accent = moras[0].vowel.accent_position();
  accent = accent <= moras.size() ? accent : moras.size();
  bool is_interrogative = moras[moras.size() - 1].vowel.is_interrogative();
  AccentPhraseLabel accent_phrase_label;
  accent_phrase_label.initialize(moras, accent, is_interrogative);
  return accent_phrase_label;
}

std::vector<Label> AccentPhraseLabel::labels() const
{
  std::vector<Label> labels;
  for (auto &&mora : this->moras)
  {
    for (auto &&label : mora.labels())
    {
      labels.emplace_back(Label(label));
    }
  }
  return labels;
}

void BreathGroupLabel::initialize(std::vector<AccentPhraseLabel> accent_phrases)
{
  this->accent_phrases = accent_phrases;
}

BreathGroupLabel BreathGroupLabel::from_labels(std::vector<Label> const &labels)
{
  std::vector<AccentPhraseLabel> accent_phrases;
  std::vector<Label> accent_labels;
  for (size_t i = 0; i < labels.size(); i++)
  {
    Label label = labels[i];
    Label next_label = i == labels.size() - 1 ? Label() : labels[i + 1];
    if (!next_label.is_valid() || label.breath_group_index() != next_label.breath_group_index() || label.accent_phrase_index() != next_label.accent_phrase_index())
    {
      AccentPhraseLabel accent_phrase = AccentPhraseLabel::from_labels(accent_labels);
      accent_phrases.emplace_back(accent_phrase);
      accent_labels.clear();
    }
  }
  BreathGroupLabel breath_group_label;
  breath_group_label.initialize(accent_phrases);
  return breath_group_label;
}

std::vector<Label> BreathGroupLabel::labels() const
{
  std::vector<Label> labels;
  for (auto &&accent_phrase : this->accent_phrases)
  {
    for (auto &&label : accent_phrase.labels())
    {
      labels.emplace_back(Label(label));
    }
  }
  return labels;
}

void UtteranceLabel::initialize(std::vector<BreathGroupLabel> breath_groups, std::vector<Label> pauses)
{
  this->breath_groups = breath_groups;
  this->pauses = pauses;
}

UtteranceLabel UtteranceLabel::from_labels(std::vector<Label> const &labels)
{
  std::vector<Label> pauses;
  std::vector<BreathGroupLabel> breath_groups;
  std::vector<Label> group_labels;
  for (auto &&_label : labels)
  {
    Label label = Label(_label);
    if (!label.is_pause())
    {
      group_labels.emplace_back(label);
    }
    else
    {
      pauses.emplace_back(label);
      if (group_labels.size() > 0)
      {
        BreathGroupLabel breath_group = BreathGroupLabel::from_labels(group_labels);
        breath_groups.emplace_back(breath_group);
        group_labels.clear();
      }
    }
  }
  UtteranceLabel utterance_label;
  utterance_label.initialize(breath_groups, pauses);
  return utterance_label;
}

std::vector<Label> UtteranceLabel::labels() const
{
  std::vector<Label> labels;
  for (size_t i = 0; i < this->pauses.size(); i++)
  {
    if (this->pauses[i].is_valid())
      labels.emplace_back(Label(this->pauses[i]));
    if (i < this->pauses.size() - 1)
    {
      for (auto &&label : this->breath_groups[i].labels())
      {
        labels.emplace_back(Label(label));
      }
    }
  }
  return labels;
}

std::string mora_to_text(std::string const &mora_phonemes)
{
  std::string mora_phonemes_copy(mora_phonemes);
  char last_mora_honemes = mora_phonemes_copy[mora_phonemes_copy.size() - 1];
  if (last_mora_honemes == 'A' || last_mora_honemes == 'I' || last_mora_honemes == 'U' || last_mora_honemes == 'E' || last_mora_honemes == 'O')
    mora_phonemes_copy[mora_phonemes_copy.size() - 1] = static_cast<char>(last_mora_honemes + 32);
  for (auto &&[phoneme, kana] : mora_phonemes_to_mora_kana)
  {
    if (mora_phonemes_copy == phoneme)
      return std::string(kana);
  }
  return mora_phonemes_copy;
}

std::vector<Mora> mora_labels_to_moras(std::vector<MoraLabel> *mora_labels)
{
  std::vector<MoraLabel> copy_mora_labels(*mora_labels);
  std::vector<Mora> moras;
  for (auto &&mora : copy_mora_labels)
  {
    std::string mora_phonemes;
    for (auto &&label : mora.labels())
    {
      mora_phonemes += label.phoneme();
    }
    moras.emplace_back(Mora{
        mora_to_text(mora_phonemes),
        mora.consonant.is_valid() ? mora.consonant.phoneme() : "",
        static_cast<float>(mora.consonant.is_valid() ? 0 : -1),
        mora.vowel.phoneme(),
        0.0,
        0.0,
    });
  }
  return moras;
}

std::vector<AccentPhrase> utterance_to_accent_phrases(UtteranceLabel *utterance)
{
  if (utterance->breath_groups.size() == 0)
    return std::vector<AccentPhrase>();
  std::vector<AccentPhrase> accent_phrases;
  for (size_t i_breath_group = 0; i_breath_group < utterance->breath_groups.size(); i_breath_group++)
  {
    BreathGroupLabel breath_group = utterance->breath_groups[i_breath_group];
    for (size_t i_accent_phrase = 0; i_accent_phrase < breath_group.accent_phrases.size(); i_accent_phrase++)
    {
      AccentPhraseLabel accent_phrase = breath_group.accent_phrases[i_accent_phrase];
      AccentPhrase accent_phrase_new;
      std::vector<Mora> moras_temp = mora_labels_to_moras(&accent_phrase.moras);
      for (auto &&mora_temp : moras_temp)
      {
        accent_phrase_new.moras->emplace_back(mora_temp);
      }
      accent_phrase_new.accent = accent_phrase.accent;
      if (i_accent_phrase == breath_group.accent_phrases.size() - 1 && i_breath_group != utterance->breath_groups.size() - 1)
      {
        accent_phrase_new.pause_mora->text = "、";
        accent_phrase_new.pause_mora->consonant = "";
        accent_phrase_new.pause_mora->consonant_length = -1;
        accent_phrase_new.pause_mora->vowel = "pau";
        accent_phrase_new.pause_mora->vowel_length = 0;
        accent_phrase_new.pause_mora->pitch = 0;
      }
      else
      {
        accent_phrase_new.pause_mora = nullptr;
      }
      accent_phrase_new.is_interrogative = accent_phrase.is_interrogative;
      accent_phrases.emplace_back(accent_phrase_new);
    }
  }
  return accent_phrases;
}

std::vector<AccentPhrase> text_to_accent_phrases(std::string const &text)
{
  std::string copy_text(text);
  // textの空白を取り除く
  if (copy_text.size() == 0)
    return std::vector<AccentPhrase>();
  std::vector<std::string> features; // = text_to_features(copy_text);
  std::vector<Label> labels;
  for (size_t i = 0; i < features.size(); i++)
  {
    labels.emplace_back(Label::from_feature(features[i]));
  }
  UtteranceLabel utterance = UtteranceLabel::from_labels(labels);
  return utterance_to_accent_phrases(&utterance);
}
