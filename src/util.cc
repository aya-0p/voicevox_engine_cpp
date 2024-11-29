#include <vector>
#include <cstdint>
#include <stdlib.h>
#include <string>
#include "./util.h"
#include <filesystem>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <format>

void vec2arr(std::vector<int64_t> *vec, int64_t first, int64_t last, int64_t *arr)
{
  arr[0] = first;
  for (size_t i = 0; i < vec->size(); i++)
  {
    arr[i + 1] = (*vec)[i];
  }
  arr[vec->size() + 1] = last;
  return;
}

void vec2arr(std::vector<int64_t> *vec, int64_t *arr)
{
  for (size_t i = 0; i < vec->size(); i++)
  {
    arr[i] = (*vec)[i];
  }
  return;
}

std::vector<int64_t> arr2vec(int64_t *arr, int64_t size)
{
  std::vector<int64_t> vec(size, 0);
  for (size_t i = 0; i < vec.size(); i++)
  {
    vec[i] = arr[i];
  }
  return vec;
}

void vec2arr(std::vector<float> *vec, float first, float last, float *arr)
{
  arr[0] = first;
  for (size_t i = 0; i < vec->size(); i++)
  {
    arr[i + 1] = (*vec)[i];
  }
  arr[vec->size() - 1] = last;
  return;
}

void vec2arr(std::vector<float> *vec, float *arr)
{
  for (size_t i = 0; i < vec->size(); i++)
  {
    arr[i] = (*vec)[i];
  }
  return;
}

std::vector<float> arr2vec(float *arr, int64_t size)
{
  std::vector<float> vec(size, 0.0f);
  for (size_t i = 0; i < vec.size(); i++)
  {
    vec[i] = arr[i];
  }
  return vec;
}

std::filesystem::path get_home_dir()
{
  const char *homedir;

  if ((homedir = getenv("HOME")) == NULL)
  {
    homedir = getpwuid(getuid())->pw_dir;
  }
  return std::filesystem::path(homedir);
}

std::string platform_system()
{
#ifdef _WIN32
  return "Windows";
#elif __MACH__
  return "Darwin";
#elif __linux__
  return "Linux";
#else
  return "Unknown";
#endif
}

std::string get_env(std::string const &name)
{
  return std::string(getenv(name.c_str()));
}

std::string user_data_dir(std::string const &appname)
{
#ifdef _WIN32
  return "Windows";
  char *path = reinterpret_cast<char *>(calloc(256, 1));
  SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, path);
  return std::string(std::filesystem::path(path) / appname);
#elif __MACH__
  return std::string(get_home_dir() / "Library/Application Support" / appname);
#else
  return std::string(get_home_dir() / ".local/share" / appname);
#endif
}

std::vector<std::vector<float>> numpy_repeat(std::vector<std::vector<float>> *a, std::vector<int64_t> *repeats)
{
  size_t length = std::min(a->size(), repeats->size());
  size_t size = 0;
  for (size_t i = 0; i < length; i++)
  {
    size += (*repeats)[i];
  }
  std::vector<std::vector<float>> vec = std::vector<std::vector<float>>(size, std::vector<float>());
  size_t t = 0;
  for (size_t i = 0; i < length; i++)
  {
    for (size_t j = 0; j < (*repeats)[i]; j++)
    {
      vec[t] = std::vector<float>((*a)[i]);
      t++;
    }
  }
  return vec;
}
std::vector<float> numpy_repeat(std::vector<float> *a, std::vector<int64_t> *repeats)
{
  size_t length = std::min(a->size(), repeats->size());
  size_t size = 0;
  for (size_t i = 0; i < length; i++)
  {
    size += (*repeats)[i];
  }
  std::vector<float> vec = std::vector<float>(size, 0.0f);
  size_t t = 0;
  for (size_t i = 0; i < length; i++)
  {
    for (size_t j = 0; j < (*repeats)[i]; j++)
    {
      vec[t] = (*a)[i];
      t++;
    }
  }
  return vec;
}
std::vector<int64_t> numpy_repeat(std::vector<int64_t> *a, std::vector<int64_t> *repeats)
{
  size_t length = std::min(a->size(), repeats->size());
  size_t size = 0;
  for (size_t i = 0; i < length; i++)
  {
    size += (*repeats)[i];
  }
  std::vector<int64_t> vec = std::vector<int64_t>(size, 0);
  size_t t = 0;
  for (size_t i = 0; i < length; i++)
  {
    for (size_t j = 0; j < (*repeats)[i]; j++)
    {
      vec[t] = (*a)[i];
      t++;
    }
  }
  return vec;
}

bool vector_has(std::vector<std::string> *vec, std::string const &data)
{
  for (auto &&ve : *vec)
  {
    if (data == ve)
      return true;
  }
  return false;
}
std::vector<std::string> copy_strings(std::vector<std::string> *vec)
{
  std::vector<std::string> vec_new = std::vector<std::string>(vec->size(), "");
  for (size_t i = 0; i < vec->size(); i++)
  {
    vec_new[i] = std::string((*vec)[i]);
  }
  return vec_new;
}

std::vector<int64_t> numpy_concatenate(std::vector<std::vector<int64_t>> *arrays)
{
  size_t length = 0;
  for (auto &&arr : *arrays)
  {
    length += arr.size();
  }

  std::vector<int64_t> vec = std::vector<int64_t>(length, 0);
  size_t i = 0;
  for (auto &&t_vec_1 : *arrays)
  {
    for (auto &&t_vec_2 : t_vec_1)
    {
      vec[i] = t_vec_2;
      i++;
    }
  }
  return vec;
}

std::vector<float> drop_edge(std::vector<float> *vec)
{
  std::vector<float> new_vec = std::vector<float>(vec->size() - 2, 0.0f);
  for (size_t i = 0; i < vec->size() - 2; i++)
  {
    new_vec[i] = (*vec)[i + 1];
  }
  return new_vec;
}

std::string to_string(float in)
{
  return std::format("{:.10}", in);
}

std::string to_string(int64_t in)
{
  return std::to_string(in);
}

std::string to_string(int32_t in)
{
  return std::to_string(in);
}

std::string to_string(bool in)
{
  return in ? "true" : "false";
}
