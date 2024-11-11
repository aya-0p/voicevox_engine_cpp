#include <malloc.h>
#include <unistd.h>
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#endif
#include "./runtime_utility.h"
#include "../util.h"

#define PATH_SIZE 256

std::string engine_root()
{
#ifdef _WIN32
  char *path = calloc(PATH_SIZE, sizeof(char));
  GetModuleFileNameA(NULL, path, PATH_SIZE);
  return std::filesystem::path(path).parent_path().parent_path();
#elif __MACH__
  uint32_t bufsize = PATH_SIZE;
  char *path = calloc(PATH_SIZE, sizeof(char));

  _NSGetExecutablePath(path, &bufsize);
  return std::filesystem::path(path).parent_path().parent_path();
#elif __linux__
  char *path = reinterpret_cast<char *>(calloc(PATH_SIZE, sizeof(char)));
  char *link = reinterpret_cast<char *>(calloc(PATH_SIZE, sizeof(char)));

  snprintf(link, PATH_SIZE, "/proc/%d/exe", getpid());

  readlink(link, path, PATH_SIZE);
  free(link);
  return std::filesystem::path(path).parent_path().parent_path();
#endif
}

std::string resource_root()
{
  return std::filesystem::path(engine_root()) / "resources";
}

std::string engine_manifest_path()
{
  return std::filesystem::path(engine_root()) / "engine_manifest.json";
}

std::string get_save_dir()
{
  if (is_development())
  {
    return user_data_dir("voicevox-engine-dev");
  }
  else
  {
    return user_data_dir("voicevox-engine");
  }
}
