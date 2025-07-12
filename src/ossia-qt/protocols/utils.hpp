#pragma once
#include <ossia-qt/invoke.hpp>
#include <ossia-qt/js_utilities.hpp>

#include <boost/asio/io_context.hpp>

#include <QPointer>

#define run_on_qt_thread(...) \
  ossia::qt::run_async(this, [=, this] { __VA_ARGS__; }, Qt::AutoConnection);
#define run_on_asio_thread(...) \
  boost::asio::dispatch(this->context(), [=, self = QPointer{this}] { __VA_ARGS__ });
