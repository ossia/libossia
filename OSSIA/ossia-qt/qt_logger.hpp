#pragma once
#include <ossia/detail/logger.hpp>
#include <wobjectdefs.h>

#include <QMetaType>
#include <QObject>
#include <iostream>

Q_DECLARE_METATYPE(spdlog::level::level_enum)
W_REGISTER_ARGTYPE(spdlog::level::level_enum)

namespace ossia::qt
{
//! Converts log messages from spdlog to Qt signals
class OSSIA_EXPORT log_sink final
    : public QObject
    , public spdlog::sinks::sink
{
  W_OBJECT(log_sink)

public:
  log_sink() = default;
  ~log_sink() override;

  void set_pattern(const std::string &pattern) override { }
  void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override { }
  void log(const spdlog::details::log_msg& msg) override
  {
    std::cerr.write(msg.raw.data(), msg.raw.size());
    std::cerr << std::endl;
    l(msg.level,
      QString::fromUtf8(msg.raw.data(), msg.raw.size()));
  }

  void flush() override
  {
  }

  void l(spdlog::level::level_enum arg_1, const QString& arg_2) W_SIGNAL(l, arg_1, arg_2);
};
}
