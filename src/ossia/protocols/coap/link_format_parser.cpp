#include "link_format_parser.hpp"

#include <boost/spirit/home/x3.hpp>

namespace ossia::coap
{

namespace
{
static void decode_uri(const char* src, int src_n, char* dst, int dst_n) noexcept
{
  static constexpr auto digit = [](char c) constexpr {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
  };

  char a{}, b{};
  while(src_n > 2 && dst_n > 0)
  {
    if((*src == '%') && ((a = src[1]) && (b = src[2])) && (digit(a) && digit(b)))
    {
      if(a >= 'a')
        a -= 'a' - 'A';
      if(a >= 'A')
        a -= ('A' - 10);
      else
        a -= '0';
      if(b >= 'a')
        b -= 'a' - 'A';
      if(b >= 'A')
        b -= ('A' - 10);
      else
        b -= '0';
      *dst++ = 16 * a + b;
      dst_n--;
      src += 3;
      src_n -= 3;
    }
    else if(*src == '+')
    {
      *dst++ = ' ';
      dst_n--;
      src++;
      src_n--;
    }
    else
    {
      *dst++ = *src++;
      src_n--;
      dst_n--;
    }
  }

  while(src_n > 0 && dst_n > 0)
  {
    if(*src == '+')
    {
      *dst++ = ' ';
      src++;
    }
    else
    {
      *dst++ = *src++;
    }
    dst_n--;
    src_n--;
  }
  *dst++ = '\0';
}

struct actions
{
  link_format res;
  link_format::resource cur_resource;

  void begin_resource(std::string_view x)
  {
    static thread_local std::string temp;
    temp.clear();
    temp.resize(x.size() * 3 + 16);
    decode_uri(x.data(), x.size(), temp.data(), temp.size());
    cur_resource = {.path = std::string(temp.data()), .options = {}};
  }

  void end_resource(const auto&...)
  {
    res.resources.push_back(std::move(cur_resource));
    cur_resource = {};
  }

  void pair_option(const auto& res)
  {
    auto pair_name = at_c<0>(res);
    auto pair_value_opt = at_c<1>(res);
    if(pair_value_opt)
    {
      auto& pair_value = *pair_value_opt;
      switch(pair_value.which())
      {
        case 0:
          cur_resource.options[pair_name] = boost::get<std::string>(pair_value);
          break;
        case 1:
          cur_resource.options[pair_name] = boost::get<int64_t>(pair_value);
          break;
      }
    }
    else
    {
      cur_resource.options[pair_name] = ossia::monostate{};
    }
  }
};

// clang-format off
using namespace boost::spirit;

#define EVENT(e) ([](auto& ctx) { x3::get<actions>(ctx).e(x3::_attr(ctx)); })
template <typename T> struct as_type {
  auto operator()(auto p) const { return x3::rule<struct Tag, T>{} = p; }
};
static constexpr as_type<std::string> as_string{};

static const auto resource_identifier_char = x3::alnum | x3::char_('_') | x3::char_('/') | x3::char_('%') | x3::char_('-') | x3::char_('+');
static const auto resource_identifier    = as_string(+resource_identifier_char);
static const auto resource_name = '<' >> resource_identifier[EVENT(begin_resource)] >> '>';

static const auto option_identifier_char = x3::alnum | x3::char_('_');
static const auto option_identifier = as_string(+option_identifier_char);
static const auto str_option = x3::lexeme['"' >> as_string(+((x3::char_ - '"') | x3::lit("\"\""))) >> '"'];
static const auto num_option = x3::int64;
static const auto option_value = (str_option | num_option);
static const auto option = (option_identifier >> -('=' >> option_value))[EVENT(pair_option)];

static const auto resource = (resource_name >> *(x3::lit(';') >> (option % x3::lit(';'))))[EVENT(end_resource)];
static const auto resources = (resource % x3::lit(','));
#undef EVENT
// clang-format on
}

std::optional<ossia::coap::link_format> parse_link_format(std::string_view str)
{
  ossia::coap::actions r;
  auto first = str.begin();
  auto last = str.end();
  bool res = phrase_parse(
      first, last, boost::spirit::x3::with<ossia::coap::actions>(r)[resources],
      boost::spirit::x3::ascii::space, boost::spirit::x3::skip_flag::post_skip);

  if(!res || first != last)
    return std::nullopt;
  return std::move(r.res);
}

}
