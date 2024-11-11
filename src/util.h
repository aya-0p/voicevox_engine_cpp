#ifndef VOICEVOX_ENGINE_UTIL
#define VOICEVOX_ENGINE_UTIL
#include <vector>

void vec2arr_int64_t(std::vector<int64_t> *vec, int64_t first, int64_t last, int64_t *arr);
void vec2arr_int64_t(std::vector<int64_t> *vec, int64_t *arr);
std::vector<int64_t> arr2vec_int64_t(int64_t *arr, int64_t size);

void vec2arr_float(std::vector<float> *vec, float first, float last, float *arr);
void vec2arr_float(std::vector<float> *vec, float *arr);
std::vector<float> arr2vec_float(float *arr, int64_t size);
std::string platform_system();
std::string user_data_dir(std::string const &appname);
template <typename T>
std::vector<T> numpy_repeat(std::vector<T> *a, std::vector<int64_t> *repeats);
template <typename T>
std::vector<T> numpy_repeat(std::vector<T> *a, int64_t repeats);
template <typename T>
bool vector_has(std::vector<T> *vec, T *data);
std::vector<std::string> copy_strings(std::vector<std::string> *vec);
template <typename T>
std::vector<T> numpy_concatenate(std::vector<std::vector<T>> *a);
#endif
