#pragma once
#include <string_view>
#include <stdexcept>

namespace ossia
{
struct small_string
{
    static constexpr int size() { return 64; }
    char arr[64];
    constexpr const char& operator[](int i) const { return arr[i]; }
    constexpr char& operator[](int i) { return arr[i]; }

    friend constexpr small_string operator+(small_string lhs, small_string rhs)
    {
      bool past_lhs = false;
      int k = 0;
      for(int i = 0; i < small_string::size(); i++)
      {
        if(lhs[i] == 0)
        {

          for(int k = 0; k < small_string::size(); k++)
          {
            if(i + k > small_string::size())
            {
              throw std::runtime_error("string too long");
            }

            lhs[i + k] = rhs[k];
          }
        }
      }
      return lhs;
    }

    template<std::size_t N>
    friend constexpr small_string operator+(small_string lhs, const char(&rhs)[N])
    {
      bool past_lhs = false;
      int k = 0;
      for(int i = 0; i < size(); i++)
      {
        if(lhs[i] == 0)
        {
          if(i + N > small_string::size())
          {
            throw std::runtime_error("string too long");
          }

          for(int k = 0; k < N; k++)
          {
            lhs[i + k] = rhs[k];
          }
        }
      }
      return lhs;
    }

};

constexpr small_string to_lower(std::string_view arr)
{
  if(arr.size() > small_string::size())
    throw std::runtime_error("string too long");

  small_string s{};
  int i = 0;
  for(; i < arr.size(); i++)
  {
    char in = arr[i];
    if(in <= 'Z' && in >= 'A')
      s[i] = in - ('Z' - 'z');
  }
  s[i+1] = 0;
  return s;
}

}
