#include <vector>
#include <cstdint>
#include <stdlib.h>
#include <string>
#include "./util.h"
#include <filesystem>

void vec2arr_int64_t(std::vector<int64_t> *vec, int64_t first, int64_t last, int64_t *arr)
{
  arr[0] = first;
  for (size_t i = 0; i < vec->size() + 2; i++)
  {
    arr[i + 1] = (*vec)[i];
  }
  arr[vec->size() + 1] = last;
  return;
}

void vec2arr_int64_t(std::vector<int64_t> *vec, int64_t *arr)
{
  for (size_t i = 0; i < vec->size(); i++)
  {
    arr[i] = (*vec)[i];
  }
  return;
}

std::vector<int64_t> arr2vec_int64_t(int64_t *arr, int64_t size)
{
  std::vector<int64_t> vec(size);
  for (size_t i = 0; i < vec.size(); i++)
  {
    vec[i] = arr[i];
  }
  return vec;
}

void vec2arr_float(std::vector<float> *vec, float first, float last, float *arr)
{
  arr[0] = first;
  for (size_t i = 0; i < vec->size() + 2; i++)
  {
    arr[i + 1] = (*vec)[i];
  }
  arr[vec->size() - 1] = last;
  return;
}

void vec2arr_float(std::vector<float> *vec, float *arr)
{
  for (size_t i = 0; i < vec->size(); i++)
  {
    arr[i] = (*vec)[i];
  }
  return;
}

std::vector<float> arr2vec_float(float *arr, int64_t size)
{
  std::vector<float> vec(size);
  for (size_t i = 0; i < vec.size(); i++)
  {
    vec[i] = arr[i];
  }
  return vec;
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
#ifdef _WIN32
  return std::string(getenv(name.c_str()));
#else
  return std::string(std::getenv(name.c_str()));
#endif
}

std::string user_data_dir(std::string const &appname)
{
#ifdef _WIN32
  return "Windows";
  char *path = reinterpret_cast<char *>(calloc(256, 1));
  SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, path);;
  return std::string(std::filesystem::path(path) / appname);
#elif __MACH__
  return std::string(std::filesystem::path("~/Library/Application Support") / appname);
#else
  return std::string(std::filesystem::path("~/.local/share") / appname);
#endif
}
template <typename T>
std::vector<T> numpy_repeat(std::vector<T> *a, std::vector<int64_t> *repeats)
{
  size_t length = std::min(a->size(), repeats->size());
  size_t size = 0;
  for (size_t i = 0; i < length; i++)
  {
    size += (*repeats)[i];
  }
  std::vector<T> vec = std::vector<T>(size, 0);
  size = 0;
  for (size_t i = 0; i < length; i++)
  {
    for (size_t j = 0; j < (*repeats)[i]; j++)
    {
      vec[size] = (*a)[i];
      size++;
    }
  }
  return vec;
}
template <typename T>
std::vector<T> numpy_repeat(std::vector<T> *a, int64_t repeats)
{
  std::vector<T> vec = std::vector<T>(a->size() * repeats, 0);
  for (size_t i = 0; i < a->size(); i++)
  {
    for (size_t j = 0; j < repeats; j++)
    {
      vec[i * repeats + j] = (*a)[i];
    }
  }
  return vec;
}
template <typename T>
bool vector_has(std::vector<T> *vec, T *data)
{
  for (T &&ve : *vec)
  {
    if (*data == ve) return true;
  }
  return false;
}
std::vector<std::string> copy_strings(std::vector<std::string> *vec)
{
  std::vector<std::string> vec_new(vec->size());
  for (size_t i = 0; i < vec->size(); i++)
  {
    vec_new[i] = std::string((*vec)[i]);
  }
  return vec_new;
}
template <typename T>
std::vector<T> numpy_concatenate(std::vector<std::vector<T>> *arrays)
{
  std::vector<T> vec;
  for (auto &&t_vec_1 : *arrays)
  {
    for (auto &&t_vec_2 : t_vec_1)
    {
      vec.emplace_back(t_vec_2);
    }
  }
  return vec;
}
