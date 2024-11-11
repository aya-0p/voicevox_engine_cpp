#ifndef VOICEVOX_ENGINE_TTS_MODEL
#define VOICEVOX_ENGINE_TTS_MODEL
#include <string>
#include <vector>
#include <cstdint>
#include "../defines.h"

typedef struct Mora
{
  std::string text = "";
  // null? -> consonant.empty()
  std::string consonant = "";
  // null? -> -1
  float consonant_length = 0;
  std::string vowel = "";
  float vowel_length = -1;
  float pitch = 0;
} Mora;

Mora copy_mora(Mora *mora);

typedef struct AccentPhrase
{
  std::vector<Mora> *moras = new std::vector<Mora>();
  int32_t accent = 0;
  Mora *pause_mora = new Mora();
  bool is_interrogative = false;
} AccentPhrase;

AccentPhrase copy_accent_phrase(AccentPhrase *accent_phrase);

typedef struct Note
{
  // null? -> id.empty()
  NoteId id = "";
  // null? -> -1
  int32_t key = -1;
  int32_t frame_length = 0;
  std::string lyric = "";
} Note;

Note copy_note(Note *note);

typedef struct Score
{
  std::vector<Note> *notes = new std::vector<Note>();
} Score;

Score copy_score(Score *score);

typedef struct FramePhoneme
{
  std::string phoneme = "";
  int32_t frame_length = 0;
  // null? -> note_id.empty()
  NoteId note_id = "";
} FramePhoneme;

FramePhoneme copy_frame_phoneme(FramePhoneme *frame_phoneme);

typedef struct FrameAudioQuery
{
  std::vector<float> *f0 = new std::vector<float>();
  std::vector<float> *volume = new std::vector<float>();
  std::vector<FramePhoneme> *phonemes = new std::vector<FramePhoneme>();
  float volumeScale = 0;
  int32_t outputSamplingRate = 0;
  bool outputStereo = false;
} FrameAudioQuery;

FrameAudioQuery copy_frame_audio_query(FrameAudioQuery *frame_audio_query);
#endif
