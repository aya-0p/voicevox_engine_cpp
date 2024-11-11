#include "./core_version_utility.h"
#include "../util.h"
#include <algorithm>

bool is_all_number(std::string &str)
{
  bool result = true;
  for (size_t i = 0; i < str.length(); i++)
  {
    if (str[i] < '0' || '9' < str[i])
    {
      result = false;
      break;
    }
  }
  return result;
}

std::string get_latest_version(std::vector<std::string> *versions)
{
  if (versions->size() == 0)
  {
    return std::string();
  }
  std::vector<std::string> versions_copy = copy_strings(versions);
  std::sort(versions_copy.begin(), versions_copy.end(), [](std::string &a, std::string &b)
            {
              std::vector<std::string> a_split;
              std::vector<std::string> b_split;
              std::string a_temp;
              std::string b_temp;
              for (auto &&ch : a)
              {
                if (ch == '.')
                {
                  a_split.emplace_back(a_temp);
                  a_temp = std::string();
                }
                else
                {
                  a_temp += ch;
                }
              }
              if (a_temp.size() != 0) a_split.emplace_back(a_temp);
              for (auto &&ch : b)
              {
                if (ch == '.')
                {
                  b_split.emplace_back(b_temp);
                  b_temp = std::string();
                }
                else
                {
                  b_temp += ch;
                }
              }
              if (b_temp.size() != 0) b_split.emplace_back(b_temp);
              for (size_t i = 0; i < std::min(a_split.size(), b_split.size()); i++)
              {
                if (a == b) continue;
                if (is_all_number(a) && is_all_number(b)) return std::stoll(a) < std::stoll(b);
                else return a < b;
              }
              return a_split.size() < b_split.size(); });
  return std::string(*versions_copy.begin());
}
