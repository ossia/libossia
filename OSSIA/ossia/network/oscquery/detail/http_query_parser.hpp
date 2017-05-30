#pragma once
#include <ossia/detail/string_map.hpp>
#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/home/qi/directive/omit.hpp>
#include <boost/fusion/include/std_pair.hpp>

/**
 * \file http_query_parser.hpp
 *
 * Utilities to parse http GET-like requests's query part, e.g.
 * everything after '?' in the following string:
 *
 * `/foo/bar?mrskeltal=dootdoot&hint=1234`
 *
 */
namespace ossia
{
namespace oscquery
{

// query_grammar : taken from https://github.com/ssiloti/http/blob/master/http/parsers/request.hpp
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
template <typename Iterator>
struct query_grammar : public boost::spirit::qi::grammar<Iterator, string_map<std::string>()>
{
    query_grammar() : query_grammar::base_type(query_string)
    {
        using namespace boost::spirit;

        query_string = (+qchar >> -(qi::lit('=') >> +qchar)) % qi::lit("&:");
        qchar = ~qi::char_("&:=");
    }

    boost::spirit::qi::rule<Iterator, string_map<std::string>()> query_string;
    boost::spirit::qi::rule<Iterator, char()> qchar;
};

inline string_map<std::string> parse_http_methods_encoded(ossia::string_view str)
{
  // TODO a vector would be more efficient.
   string_map<std::string> methods;
   boost::spirit::qi::parse(
         str.cbegin(), str.cend(),
         query_grammar<ossia::string_view::const_iterator>(),
         methods);
   return methods;
}

//! url_decode taken from boost
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0.
inline bool url_decode(const std::string& in, std::string& out)
{
  out.clear();
  out.reserve(in.size());
  const int N = in.size();
  for (int i = 0; i < N; ++i)
  {
    switch(in[i])
    {
      case '%':
      {
        if (i + 3 <= N)
        {
          int value = 0;
          std::istringstream is(in.substr(i + 1, 2));
          if (is >> std::hex >> value)
          {
            out += static_cast<char>(value);
            i += 2;
          }
          else
          {
            return false;
          }
        }
        else
        {
          return false;
        }
        break;
      }
      case '+':
        out += ' ';
        break;
      case '\0':
        return true;
      default:
        out += in[i];
        break;
    }

  }
  return true;
}
}
}
