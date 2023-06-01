#pragma once

#include <QMetaObject>

namespace ossia::qt
{

template <typename T, typename Fun>
inline void run_async(T* self, Fun&& fun)
{
  QMetaObject::invokeMethod(self, std::forward<Fun>(fun), Qt::QueuedConnection);
}

}
