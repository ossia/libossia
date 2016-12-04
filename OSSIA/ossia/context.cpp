#include "context.hpp"

#if defined(QT_QML_LIB)
#include <qqml.h>
#include <ossia/network/common/js_utilities.hpp>
#endif

namespace ossia
{
context::context()
{
#if defined(QT_QML_LIB)
  qmlRegisterUncreatableType<ossia::net::qml_context>("org.ossia", 1, 0, "Ossia", "");
#endif
}

context::~context() = default;
}
