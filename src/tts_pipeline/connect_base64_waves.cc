#include <cstdint>
#include "./connect_base64_waves.h"
#include "../libraries/base64.hpp"
#include "../libraries/AudioFile.h"

std::vector<AudioFile<double>> decode_base64_waves(std::vector<std::string> *waves)
{
  std::vector<AudioFile<double>> waves_sr;
  for (auto &&wave : *waves)
  {
    std::vector<uint8_t> wav_bin;
    uint32_t size;
    try
    {
      std::string wave_string = base64::from_base64(wave);
      size = wave_string.size();
      wav_bin = std::vector<uint8_t>(size);
      for (size_t i = 0; i < size; i++)
      {
        wav_bin[i] = wave_string[i];
      }
    }
    catch (const std::exception &e)
    {
      throw "base64デコードに失敗しました";
    }
    AudioFile<double> audio_file;
    if (!audio_file.loadFromMemory(wav_bin))
    {
      throw "wavファイルを読み込めませんでした";
    }
    waves_sr.emplace_back(audio_file);
  }
  return waves_sr;
}

std::pair<std::vector<std::vector<double>>, int32_t> connect_base64_waves(std::vector<std::string> *waves)
{
  auto waves_sr = decode_base64_waves(waves);
  uint32_t max_sampling_rate = 1;
  int32_t max_channels = 1;
  uint64_t sum_sample = 0;
  for (auto &&wave : waves_sr)
  {
    max_sampling_rate = std::max(max_sampling_rate, wave.getSampleRate());
    max_channels = std::max(max_channels, wave.getNumChannels());
    sum_sample += wave.getNumSamplesPerChannel();
  }
  assert(0 < max_channels && max_channels <= 2); // max_channels = 1 or 2
  std::vector<std::vector<double>> data(max_channels, std::vector<double>(sum_sample));
  uint64_t current_index = 0;
  for (auto &&wave : waves_sr)
  {
    if (max_sampling_rate != wave.getSampleRate())
    {
      wave.setSampleRate(max_sampling_rate);
    }
    if (max_channels != wave.getNumChannels())
    {
      wave.setNumChannels(max_channels);
    }
    for (size_t i = 0; i < wave.getNumChannels(); i++)
    {
      for (size_t j = 0; j < wave.getNumSamplesPerChannel(); j++)
      {
        data[i][current_index] = wave.samples[i][j];
        current_index++;
      }
    }
  }
  return {data, max_sampling_rate};
}
