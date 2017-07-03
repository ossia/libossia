#include "qml_logger.hpp"
#include <ossia/network/common/websocket_log_sink.hpp>
#include <spdlog/spdlog.h>
#include <QFileInfo>

namespace ossia
{
namespace qt
{

static std::shared_ptr<spdlog::logger> m_globalQtLogger;
qml_logger::qml_logger()
  : m_logger{spdlog::get("ossia")}
  , m_appName{"The App"}
  , m_loggerHost{"ws://127.0.0.1:1337"}
{
  m_globalQtLogger = m_logger;
}

qml_logger::~qml_logger()
{

}

qml_logger& qml_logger::instance()
{
  static qml_logger logger;
  return logger;
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

bool qml_logger::logQtMessages() const
{
  return m_logQtMessages;
}

quint32 qml_logger::heartbeat() const
{
  return m_heartbeatDur;
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
  m_globalQtLogger = m_logger;
  if(m_heartbeat > 0)
  {
    m_heartbeat = std::make_shared<websocket_heartbeat>(m_ws, std::chrono::seconds(m_heartbeatDur));
  }
}

void qml_logger::trace(const QString& s) {
  if(m_logger) m_logger->trace("{}", s.toStdString());
}
void qml_logger::info(const QString& s) {
  if(m_logger) m_logger->info("{}", s.toStdString());
}
void qml_logger::debug(const QString& s) {
  if(m_logger) m_logger->debug("{}", s.toStdString());
}
void qml_logger::warning(const QString& s) {
  if(m_logger) m_logger->warn("{}", s.toStdString());
}
void qml_logger::error(const QString& s) {
  if(m_logger) m_logger->error("{}", s.toStdString());
}
void qml_logger::critical(const QString& s) {
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

static void LogQtToOssia(
    QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
  if(!m_globalQtLogger)
    return;

  auto basename_arr = QFileInfo(context.file).baseName().toUtf8();
  auto filename = basename_arr.constData();

  QByteArray localMsg = msg.toLocal8Bit();
  switch (type)
  {
    case QtDebugMsg:
      m_globalQtLogger->debug("{} ({}:{})",
                              localMsg.constData(),
                              filename,
                              context.line);
      break;
    case QtInfoMsg:
      m_globalQtLogger->info("{} ({}:{})",
                              localMsg.constData(),
                              filename,
                              context.line);
      break;
    case QtWarningMsg:
      m_globalQtLogger->warn("{} ({}:{})",
                              localMsg.constData(),
                              filename,
                              context.line);
      break;
    case QtCriticalMsg:
      m_globalQtLogger->error("{} ({}:{})",
                              localMsg.constData(),
                              filename,
                              context.line);
      break;
    case QtFatalMsg:
      m_globalQtLogger->critical("{} ({}:{})",
                              localMsg.constData(),
                              filename,
                              context.line);
  }
}

void qml_logger::setLogQtMessages(bool logQtMessages)
{
  if (m_logQtMessages == logQtMessages)
    return;

  m_logQtMessages = logQtMessages;
  emit logQtMessagesChanged(m_logQtMessages);

  if(m_logQtMessages)
  {
    qInstallMessageHandler(LogQtToOssia);
  }
  else
  {
    qInstallMessageHandler(nullptr);
  }
}

void qml_logger::setHeartbeat(quint32 heartbeat)
{
  if (m_heartbeatDur == heartbeat)
    return;

  m_heartbeatDur = heartbeat;
  emit heartbeatChanged(m_heartbeatDur);
  connectLogger();
}

}

}
