#pragma once
#include <hopscotch_map.h>
#include <mutex>

namespace ossia
{
namespace net
{
class address_base;
struct listened_addresses
{
public:
  using map_type = tsl::hopscotch_map<std::string, ossia::net::address_base*>;

  ossia::net::address_base* find(const std::string& path)
  {
    std::lock_guard<std::mutex> lock(mListeningMutex);
    auto it = mListening.find(path);
    if (it != mListening.end())
    {
      return it->second;
    }
    else
    {
      return nullptr;
    }
  }

  void insert(const map_type::value_type& m)
  {
    std::lock_guard<std::mutex> lock(mListeningMutex);
    mListening.insert(m);
  }

  void insert(map_type::value_type&& m)
  {
    std::lock_guard<std::mutex> lock(mListeningMutex);
    mListening.insert(std::move(m));
  }

  void erase(const map_type::key_type& m)
  {
    std::lock_guard<std::mutex> lock(mListeningMutex);
    mListening.erase(m);
  }

private:
  std::mutex mListeningMutex;
  map_type mListening;
};

}
}
