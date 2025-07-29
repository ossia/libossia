#pragma once

#include <ossia/detail/small_vector.hpp>
#include <span>

#include <memory>
#include <string>

namespace ossia
{
// Used in nodes
using audio_channel = ossia::pod_vector<double>;
using audio_vector = ossia::small_vector<audio_channel, 2>;

// Used for audio files
using audio_sample = float;
using audio_array = ossia::small_vector<ossia::pod_vector<audio_sample>, 2>;
template <typename T>
using audio_span = ossia::small_vector<std::span<const T>, 8>;
template <typename T>
using mutable_audio_span = ossia::small_vector<std::span<T>, 8>;

struct audio_data
{
  audio_array data;
  std::string path;
  int rate{};
};

#if BOOST_VERSION >= 107200
static_assert(noexcept(audio_data{}));
#endif

using audio_handle = std::shared_ptr<audio_data>;
struct drwav_handle;
/*
struct video_data
{
};

struct mmap_audio_handle
{
  drwav* data{};
};

struct media_data
{
  std::string file_path;
  ossia::variant<audio_handle, mmap_audio_handle> media;
};*/
}

#if __has_include(<QDebug>)
#if defined(QT_CORE_LIB)
#include <QDebug>

#include <algorithm>
inline QDebug operator<<(QDebug s, const ossia::audio_channel& v)
{
  auto& q = s.noquote().nospace();
  q << v.size() << ": ";
  std::size_t n = v.size();
  if(n >= 64)
  {
    double min = *std::min_element(v.begin(), v.end());
    double max = *std::max_element(v.begin(), v.end());
    if(min == max)
    {
      q << "________________________________________________________________";
    }
    else
    {
      for(std::size_t i = 0; (i * n / 64) < v.size(); i++)
      {
        double val = (v[i * n / 64] - min) / (max - min);
        const char* c = "_";
        if(val <= 1. / 8.)
          c = "▁";
        else if(val <= 2. / 8.)
          c = "▂";
        else if(val <= 3. / 8.)
          c = "▃";
        else if(val <= 4. / 8.)
          c = "▄";
        else if(val <= 5. / 8.)
          c = "▅";
        else if(val <= 6. / 8.)
          c = "▆";
        else if(val <= 7. / 8.)
          c = "▇";
        else
          c = "█";
        q.noquote() << c;
      }
    }
  }
  return q;
}
inline QDebug operator<<(QDebug s, const ossia::audio_vector& v)
{
  auto& q = s.noquote().nospace();
  q << v.size() << ": \n";
  for(auto& chan : v)
  {
    q << "[ " << chan << " ], \n";
  }
  return q;
}

#include <span>
inline QDebug operator<<(QDebug s, const std::span<float>& v)
{
  auto& q = s.noquote().nospace();
  q << v.size() << ": ";
  std::size_t n = v.size();
  if(n >= 64)
  {
    double min = *std::min_element(v.begin(), v.end());
    double max = *std::max_element(v.begin(), v.end());
    if(min == max)
    {
      q << "________________________________________________________________";
    }
    else
    {
      for(std::size_t i = 0; (i * n / 64) < v.size(); i++)
      {
        double val = (v[i * n / 64] - min) / (max - min);
        const char* c = "_";
        if(val <= 1. / 8.)
          c = "▁";
        else if(val <= 2. / 8.)
          c = "▂";
        else if(val <= 3. / 8.)
          c = "▃";
        else if(val <= 4. / 8.)
          c = "▄";
        else if(val <= 5. / 8.)
          c = "▅";
        else if(val <= 6. / 8.)
          c = "▆";
        else if(val <= 7. / 8.)
          c = "▇";
        else
          c = "█";
        q.noquote() << c;
      }
    }
  }
  return q;
}
inline QDebug operator<<(QDebug s, const ossia::small_vector<std::span<float>, 8>& v)
{
  auto& q = s.noquote().nospace();
  q << v.size() << ": \n";
  for(auto& chan : v)
  {
    q << "[ " << chan << " ], \n";
  }
  return q;
}

#endif
#endif
