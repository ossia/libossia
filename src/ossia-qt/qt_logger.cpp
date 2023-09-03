#include <ossia/detail/optional.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/network/base/name_validation.hpp>

#include <ossia-qt/qt_logger.hpp>

#include <QDebug>
#include <QString>
#include <QStringBuilder>
#include <qobjectdefs.h>

#include <wobjectimpl.h>

#include <algorithm>
#include <string>
#include <vector>
// MOVEME
//  Taken from https://stackoverflow.com/a/18230916/1495627
OSSIA_EXPORT bool latin_compare(const QString& qstr, const std::string& str)
{
  if(qstr.length() != (int)str.size())
    return false;

  const QChar* qstrData = qstr.data();
  const int N = qstr.length();
  for(int i = 0; i < N; ++i)
  {
    if(qstrData[i].toLatin1() != str[i])
      return false;
  }
  return true;
}

namespace ossia::net
{
OSSIA_EXPORT void sanitize_device_name(QString& ret)
{
  const QChar underscore = '_';
  for(auto& c : ret)
  {
    if(ossia::net::is_valid_character_for_device(c))
      continue;
    else
      c = underscore;
  }
}

OSSIA_EXPORT
void sanitize_name(QString& ret)
{
  const QChar underscore = '_';
  for(auto& c : ret)
  {
    if(ossia::net::is_valid_character_for_name(c))
      continue;
    else
      c = underscore;
  }
}

OSSIA_EXPORT
QString sanitize_name(QString name, const std::vector<QString>& brethren)
{
  sanitize_name(name);
  bool is_here = false;
  std::optional<int> name_instance;
  ossia::small_vector<int, 16> instance_num;
  const auto b_size = brethren.size();
  instance_num.reserve(b_size);

  // First get the root name : the first part of the "a.b"
  QString root_name = name;
  {
    auto pos = name.lastIndexOf('.');
    if(pos != -1)
    {
      bool res = false;
      name_instance = name.mid(pos + 1).toInt(&res);
      if(res)
        root_name = name.mid(0, pos);
    }
  }

  const auto root_len = root_name.size();
  // QStringView::toInt comes in 5.15.2

  for(QStringView n_name : brethren)
  {
    if(n_name == name)
    {
      is_here = true;
    }

    if(n_name.size() < (root_len + 1))
      continue;

    bool same_root = true;
    for(int i = 0; i < root_len; i++)
    {
      if(n_name[i] != root_name[i])
      {
        same_root = false;
        break;
      }
    }

    if(same_root && (n_name[root_len] == '.'))
    {
      // Instance
      bool b = false;
      int n = n_name.mid(root_len + 1).toInt(&b);
      if(b)
        instance_num.push_back(n);
    }
    // case where we have the "default" instance without .0
    else if(same_root && root_len == n_name.length())
    {
      instance_num.push_back(0);
    }
  }

  if(!is_here)
  {
    return name;
  }
  else
  {
    auto n = instance_num.size();
    if((n == 0) || ((n == 1) && (instance_num[0] == 0)))
    {
      return root_name + QStringLiteral(".1");
    }
    else
    {
      std::sort(instance_num.begin(), instance_num.end());
      return root_name % "." % QString::number(instance_num.back() + 1);
    }
  }
}
}

namespace ossia::qt
{
void log_sink::log(const spdlog::details::log_msg& msg)
{
  auto logger
      = QMessageLogger(msg.source.filename, msg.source.line, msg.source.funcname);

  switch(msg.level)
  {
    case spdlog::level::level_enum::off:
      break;
    case spdlog::level::level_enum::trace:
      logger.info("[ossia] %.*s", (int)msg.payload.size(), msg.payload.data());
      break;
    case spdlog::level::level_enum::info:
      logger.info("[ossia] %.*s", (int)msg.payload.size(), msg.payload.data());
      break;
    case spdlog::level::level_enum::debug:
      logger.debug("[ossia] %.*s", (int)msg.payload.size(), msg.payload.data());
      break;
    case spdlog::level::level_enum::warn:
      logger.warning("[ossia] %.*s", (int)msg.payload.size(), msg.payload.data());
      break;
    case spdlog::level::level_enum::err:
      logger.critical("[ossia] %.*s", (int)msg.payload.size(), msg.payload.data());
      break;
    case spdlog::level::level_enum::critical:
      logger.critical("[ossia] %.*s", (int)msg.payload.size(), msg.payload.data());
      break;
    default:
      break;
  }
}
}
W_OBJECT_IMPL(ossia::qt::log_sink)
ossia::qt::log_sink::~log_sink() = default;
