#include "qml_logger.hpp"
#include <ossia/network/common/websocket_log_sink.hpp>
#include <spdlog/spdlog.h>

namespace ossia
{
namespace qt
{
qml_logger::qml_logger()
  : m_logger{spdlog::get("ossia")}
  , m_appName{"The App"}
  , m_loggerHost{"ws://127.0.0.1:1337"}
{

}

QString qml_logger::appName() const
{
  return m_appName;
}

QString qml_logger::appVersion() const
{
  return m_appVersion;
}

QString qml_logger::appCreator() const
{
  return m_appCreator;
}

QString qml_logger::loggerHost() const
{
  return m_loggerHost;
}

void qml_logger::setAppName(QString appAuthor)
{
  if (m_appName == appAuthor)
    return;

  m_appName = appAuthor;
  emit appNameChanged(appAuthor);
  connectLogger();
}

void qml_logger::setAppVersion(QString appVersion)
{
  if (m_appVersion == appVersion)
    return;

  m_appVersion = appVersion;
  emit appVersionChanged(appVersion);
}

void qml_logger::setAppCreator(QString appCreator)
{
  if (m_appCreator == appCreator)
    return;

  m_appCreator = appCreator;
  emit appCreatorChanged(appCreator);
}

void qml_logger::connectLogger()
{
  using namespace std::literals;
  m_heartbeat.reset();
  m_logger.reset();
  m_ws.reset();
  if(m_loggerHost.isEmpty())
    return;
  if(m_appName.isEmpty())
    return;

  m_ws = std::make_shared<websocket_threaded_connection>(
           m_loggerHost.toStdString());
  m_logger = std::make_shared<spdlog::logger>(
               "qml-logger",
               std::make_shared<websocket_log_sink>(m_ws,
               m_appName.toStdString()));
  m_heartbeat = std::make_shared<websocket_heartbeat>(m_ws, 2s);
}

void qml_logger::logTrace(const QString& s) {
  if(m_logger) m_logger->trace("{}", s.toStdString());
}
void qml_logger::logInfo(const QString& s) {
  if(m_logger) m_logger->info("{}", s.toStdString());
}
void qml_logger::logDebug(const QString& s) {
  if(m_logger) m_logger->debug("{}", s.toStdString());
}
void qml_logger::logWarning(const QString& s) {
  if(m_logger) m_logger->warn("{}", s.toStdString());
}
void qml_logger::logError(const QString& s) {
  if(m_logger) m_logger->error("{}", s.toStdString());
}
void qml_logger::logCritical(const QString& s) {
  if(m_logger) m_logger->critical("{}", s.toStdString());
}

void qml_logger::setLoggerHost(QString loggerHost)
{
  if (m_loggerHost == loggerHost)
    return;

  m_loggerHost = loggerHost;
  emit loggerHostChanged(m_loggerHost);
  connectLogger();
}

}

}
