#pragma once

#include <QMetaObject>
#include <QTimer>

namespace ossia::qt
{

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
template<typename T, typename Fun>
inline void run_async(T* self, Fun&& fun)
{
  QMetaObject::invokeMethod(self, fun, Qt::QueuedConnection);
}
#else

template<typename T, typename Fun>
inline void run_async(T* self, Fun&& fun)
{
  QTimer::singleShot(0, self, fun);
}
#endif

}
