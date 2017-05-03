#include <ossia-qt/qml_context.hpp>
#include <ossia/detail/logger.hpp>
namespace ossia
{
namespace qt
{

qml_context::~qml_context()
{

}

void qml_context::logTrace(const QString& s) { ossia::logger().trace("{}", s.toStdString()); }
void qml_context::logInfo(const QString& s) { ossia::logger().info("{}", s.toStdString()); }
void qml_context::logDebug(const QString& s) { ossia::logger().debug("{}", s.toStdString()); }
void qml_context::logWarning(const QString& s) { ossia::logger().warn("{}", s.toStdString()); }
void qml_context::logError(const QString& s) { ossia::logger().error("{}", s.toStdString()); }
void qml_context::logCritical(const QString& s) { ossia::logger().critical("{}", s.toStdString()); }

}
}
