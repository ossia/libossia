#include <ossia-qt/qml_context.hpp>
#include <ossia/detail/logger.hpp>
namespace ossia
{
namespace qt
{

qml_logger::~qml_logger()
{

}

void qml_logger::logTrace(const QString& s) { ossia::logger().trace("{}", s.toStdString()); }
void qml_logger::logInfo(const QString& s) { ossia::logger().info("{}", s.toStdString()); }
void qml_logger::logDebug(const QString& s) { ossia::logger().debug("{}", s.toStdString()); }
void qml_logger::logWarning(const QString& s) { ossia::logger().warn("{}", s.toStdString()); }
void qml_logger::logError(const QString& s) { ossia::logger().error("{}", s.toStdString()); }
void qml_logger::logCritical(const QString& s) { ossia::logger().critical("{}", s.toStdString()); }

}
}
