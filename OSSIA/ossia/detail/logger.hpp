#pragma once

#define SPDLOG_NO_DATETIME
#define SPDLOG_NO_THREAD_ID
#define SPDLOG_NO_NAME

#define SPDLOG_DEBUG_ON
#define SPDLOG_TRACE_ON

#define SPDLOG_FMT_EXTERNAL
#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>
#include <fmt/ostream.h>
