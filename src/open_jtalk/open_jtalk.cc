#include "./mecab/mecab.h"
#include "./njd/njd.h"
#include "./jpcommon/jpcommon.h"
#include "./text2mecab/text2mecab.h"
#include "./mecab2njd/mecab2njd.h"
#include "./njd_set_accent_phrase/njd_set_accent_phrase.h"
#include "./njd_set_accent_type/njd_set_accent_type.h"
#include "./njd_set_digit/njd_set_digit.h"
#include "./njd_set_long_vowel/njd_set_long_vowel.h"
#include "./njd_set_pronunciation/njd_set_pronunciation.h"
#include "./njd_set_unvoiced_vowel/njd_set_unvoiced_vowel.h"
#include "./njd2jpcommon/njd2jpcommon.h"

#include <iostream>
#include <vector>
#include <string>

#include "./open_jtalk.h"

typedef struct Open_JTalk
{
  Mecab mecab;
  NJD njd;
  JPCommon jpcommon;
} Open_JTalk;

void Open_JTalk_initialize(Open_JTalk *open_jtalk)
{
  Mecab_initialize(&open_jtalk->mecab);
  NJD_initialize(&open_jtalk->njd);
  JPCommon_initialize(&open_jtalk->jpcommon);
}

bool Open_JTalk_load(Open_JTalk *open_jtalk, std::string &dn_mecab)
{
  return Mecab_load(&open_jtalk->mecab, dn_mecab);
}

std::vector<std::string> Open_JTalk_synthesis(Open_JTalk *open_jtalk, std::string const &txt)
{
  char *buff = (char *)calloc(1024, sizeof(char));
  text2mecab(buff, txt.c_str());
  Mecab_analysis(&open_jtalk->mecab, buff);
  mecab2njd(&open_jtalk->njd, Mecab_get_feature(&open_jtalk->mecab),
            Mecab_get_size(&open_jtalk->mecab));
  njd_set_pronunciation(&open_jtalk->njd);
  njd_set_digit(&open_jtalk->njd);
  njd_set_accent_phrase(&open_jtalk->njd);
  njd_set_accent_type(&open_jtalk->njd);
  njd_set_unvoiced_vowel(&open_jtalk->njd);
  njd_set_long_vowel(&open_jtalk->njd);
  njd2jpcommon(&open_jtalk->jpcommon, &open_jtalk->njd);
  JPCommon_make_label(&open_jtalk->jpcommon);
  int length = JPCommon_get_label_size(&open_jtalk->jpcommon);
  std::vector<std::string> labels_string = std::vector<std::string>(length);
  char **labels_c = JPCommon_get_label_feature(&open_jtalk->jpcommon);
  for (size_t i = 0; i < length; i++)
  {
    labels_string[i] = labels_c[i];
  }
  return labels_string;
}

std::vector<std::string> text_to_features(std::string const &text)
{
  Open_JTalk open_jtalk;
  Open_JTalk_initialize(&open_jtalk);
  std::string dn_dict = "/home/aya/open_jtalk/";
  Open_JTalk_load(&open_jtalk, dn_dict);
  return Open_JTalk_synthesis(&open_jtalk, text);
}
