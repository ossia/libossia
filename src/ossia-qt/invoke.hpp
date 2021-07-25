#pragma once

#include <QMetaObject>
#include <QTimer>

namespace ossia::qt
{

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))

template<typename T, typename Fun>
inline void run_async(T* self, Fun&& fun)
{
  QMetaObject::invokeMethod(self, std::forward<Fun>(fun), Qt::QueuedConnection);
}

#else

template<typename T, typename Fun>
inline void run_async(T* self, Fun&& fun)
{
  if(std::is_copy_constructible_v<Fun>) {
    QTimer::singleShot(0, self, std::forward<Fun>(fun));
  } else {
    // Old Qt did not support move-only types such as unique_ptr,
    // add a hack for that here
    auto f = new Fun{std::move(fun)};
    QTimer::singleShot(0, self, [f] {
      (*f)();
      delete f;
    });
  }
}

#endif

}
