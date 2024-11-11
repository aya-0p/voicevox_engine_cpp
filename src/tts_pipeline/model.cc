#include "./model.h"

Mora copy_mora(Mora *mora)
{
  Mora mora_new;
  mora_new.text = std::string(mora->text);
  mora_new.consonant = std::string(mora->consonant);
  mora_new.consonant_length = mora->consonant_length;
  mora_new.vowel = std::string(mora->vowel);
  mora_new.vowel_length = mora->vowel_length;
  mora_new.pitch = mora->pitch;
  return mora_new;
}

AccentPhrase copy_accent_phrase(AccentPhrase *accent_phrase)
{
  AccentPhrase accent_phrase_new;
  accent_phrase_new.moras->reserve(accent_phrase->moras->size());
  for (size_t i = 0; i < accent_phrase->moras->size(); i++)
  {
    accent_phrase_new.moras->emplace_back(copy_mora(&(*accent_phrase->moras)[i]));
  }
  accent_phrase_new.accent = accent_phrase->accent;
  if (accent_phrase->pause_mora == nullptr)
  {
    accent_phrase_new.pause_mora = nullptr;
  }
  else
  {
    accent_phrase_new.pause_mora->text = std::string(accent_phrase->pause_mora->text);
    accent_phrase_new.pause_mora->consonant = std::string(accent_phrase->pause_mora->consonant);
    accent_phrase_new.pause_mora->consonant_length = accent_phrase->pause_mora->consonant_length;
    accent_phrase_new.pause_mora->vowel = std::string(accent_phrase->pause_mora->vowel);
    accent_phrase_new.pause_mora->vowel_length = accent_phrase->pause_mora->vowel_length;
    accent_phrase_new.pause_mora->pitch = accent_phrase->pause_mora->pitch;
  }
  accent_phrase_new.is_interrogative = accent_phrase->is_interrogative;
  return accent_phrase_new;
}

Note copy_note(Note *note)
{
  Note note_copy;
  note_copy.id = std::string(note->id);
  note_copy.key = note->key;
  note_copy.frame_length = note->frame_length;
  note_copy.lyric = std::string(note->lyric);
  return note_copy;
}

Score copy_score(Score *score)
{
  Score score_copy;
  score_copy.notes->reserve(score->notes->size());
  for (size_t i = 0; i < score->notes->size(); i++)
  {
    score_copy.notes->emplace_back(copy_note(&(*score->notes)[i]));
  }
  return score_copy;
}

FramePhoneme copy_frame_phoneme(FramePhoneme *frame_phoneme)
{
  FramePhoneme frame_phoneme_copy;
  frame_phoneme_copy.phoneme = std::string(frame_phoneme->phoneme);
  frame_phoneme_copy.frame_length = frame_phoneme->frame_length;
  frame_phoneme_copy.note_id = std::string(frame_phoneme->note_id);
  return frame_phoneme_copy;
}

FrameAudioQuery copy_frame_audio_query(FrameAudioQuery *frame_audio_query)
{
  FrameAudioQuery frame_audio_query_new;
  frame_audio_query_new.f0->reserve(frame_audio_query->f0->size());
  for (size_t i = 0; i < frame_audio_query->f0->size(); i++)
  {
    frame_audio_query_new.f0->emplace_back((*frame_audio_query->f0)[i]);
  }
  frame_audio_query_new.volume->reserve(frame_audio_query->volume->size());
  for (size_t i = 0; i < frame_audio_query->volume->size(); i++)
  {
    frame_audio_query_new.volume->emplace_back((*frame_audio_query->volume)[i]);
  }
  frame_audio_query_new.phonemes->reserve(frame_audio_query->phonemes->size());
  for (size_t i = 0; i < frame_audio_query->phonemes->size(); i++)
  {
    frame_audio_query_new.phonemes->emplace_back(copy_frame_phoneme(&(*frame_audio_query->phonemes)[i]));
  }
  frame_audio_query_new.volumeScale = frame_audio_query->volumeScale;
  frame_audio_query_new.outputSamplingRate = frame_audio_query->outputSamplingRate;
  frame_audio_query_new.outputStereo = frame_audio_query->outputStereo;
  return frame_audio_query_new;
}
