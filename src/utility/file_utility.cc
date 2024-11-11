#include <string>
#include <filesystem>

void try_delete_file(std::string const &file_path)
{
  std::error_code err;
  std::filesystem::remove(file_path, err);
  return;
}
