#ifndef VOICEVOX_ENGINE_UTIL
#define VOICEVOX_ENGINE_UTIL
#include <vector>

void vec2arr(std::vector<int64_t> *vec, int64_t first, int64_t last, int64_t *arr);
void vec2arr(std::vector<int64_t> *vec, int64_t *arr);
std::vector<int64_t> arr2vec(int64_t *arr, int64_t size);

void vec2arr(std::vector<float> *vec, float first, float last, float *arr);
void vec2arr(std::vector<float> *vec, float *arr);
std::vector<float> arr2vec(float *arr, int64_t size);
std::string platform_system();
std::string user_data_dir(std::string const &appname);
std::vector<std::vector<float>> numpy_repeat(std::vector<std::vector<float>> *a, std::vector<int64_t> *repeats);
std::vector<float> numpy_repeat(std::vector<float> *a, std::vector<int64_t> *repeats);
std::vector<int64_t> numpy_repeat(std::vector<int64_t> *a, std::vector<int64_t> *repeats);
bool vector_has(std::vector<std::string> *vec, std::string const &data);
std::vector<std::string> copy_strings(std::vector<std::string> *vec);
std::vector<int64_t> numpy_concatenate(std::vector<std::vector<int64_t>> *a);
std::vector<float> drop_edge(std::vector<float> *vec);
std::string to_string(float in);
std::string to_string(int64_t in);
std::string to_string(int32_t in);
std::string to_string(bool in);
#endif
