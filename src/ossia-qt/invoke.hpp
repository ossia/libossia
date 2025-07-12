#pragma once
#include <ossia/detail/config.hpp>

#include <QMetaObject>

namespace ossia::qt
{
template <typename T, typename Fun>
inline void run_async(T* self, Fun&& fun, Qt::ConnectionType type = Qt::QueuedConnection)
{
  QMetaObject::invokeMethod(self, std::forward<Fun>(fun), type);
}
}
