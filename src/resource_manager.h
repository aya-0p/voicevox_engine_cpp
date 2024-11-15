#ifndef VOICEVOX_ENGINE_RESOURCE_MANAGER
#define VOICEVOX_ENGINE_RESOURCE_MANAGER
#include <string>
#include <vector>
#include <map>
#include <filesystem>

std::string b64encode_str();

class ResourceManager
{
public:
  void initialize(bool create_filemap_if_not_exist);
  void register_dir(std::filesystem::path resource_dir);
  std::string resource_str(std::filesystem::path resource_path, std::string resource_format);
  std::filesystem::path resource_path(std::string filehash);
private:
  bool create_filemap_if_not_exist = true;
  std::map<std::string, std::string> path_to_hash = std::map<std::string, std::string>();
  std::map<std::string, std::string> hash_to_path = std::map<std::string, std::string>();
};

#endif
