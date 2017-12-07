#pragma once
#include <QObject>
#include <QString>
#include <memory>
#include <ossia_export.h>
namespace spdlog
{
class logger;
}

namespace ossia
{
struct websocket_threaded_connection;
struct websocket_heartbeat;
namespace qt
{
class OSSIA_EXPORT qml_logger : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString appName READ appName WRITE setAppName NOTIFY
                 appNameChanged FINAL)
  Q_PROPERTY(QString appVersion READ appVersion WRITE setAppVersion NOTIFY
                 appVersionChanged FINAL)
  Q_PROPERTY(QString appCreator READ appCreator WRITE setAppCreator NOTIFY
                 appCreatorChanged FINAL)
  Q_PROPERTY(QString loggerHost READ loggerHost WRITE setLoggerHost NOTIFY
                 loggerHostChanged)
  Q_PROPERTY(quint32 heartbeat READ heartbeat WRITE setHeartbeat NOTIFY
                 heartbeatChanged)
  Q_PROPERTY(bool logQtMessages READ logQtMessages WRITE setLogQtMessages
                 NOTIFY logQtMessagesChanged)
  Q_PROPERTY(log_level logLevel READ logLevel WRITE setLogLevel NOTIFY
                 logLevelChanged)

  Q_PROPERTY(QStringList logFilter READ logFilter WRITE setLogFilter NOTIFY logFilterChanged)

  public:
  enum log_level
  {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5
  };
  Q_ENUM(log_level)

  qml_logger();
  ~qml_logger();

  static qml_logger& instance();
  QString appName() const;
  QString appVersion() const;
  QString appCreator() const;
  QString loggerHost() const;
  QStringList logFilter() const;

  bool logQtMessages() const;

  quint32 heartbeat() const;

  log_level logLevel() const;

signals:
  void appNameChanged(QString appName);
  void appVersionChanged(QString appVersion);
  void appCreatorChanged(QString appCreator);
  void loggerHostChanged(QString loggerHost);
  void logQtMessagesChanged(bool logQtMessages);
  void heartbeatChanged(quint32 heartbeat);
  void logLevelChanged(log_level logLevel);
  void logFilterChanged(QStringList logFilter);
  void filteredLog( int type, const QString& fileName, int line, const QString& msg);
public slots:
  void connectLogger();
  void disconnectLogger();

  void setAppName(QString appName);
  void setAppVersion(QString appVersion);
  void setAppCreator(QString appCreator);

  void trace(const QString& s);
  void info(const QString& s);
  void debug(const QString& s);
  void warning(const QString& s);
  void error(const QString& s);
  void critical(const QString& s);
  void setLoggerHost(QString loggerHost);
  void setLogQtMessages(bool logQtMessages);
  void setLogFilter(QStringList logFilter);

  void setHeartbeat(quint32 heartbeat);
  void setLogLevel(log_level);

  void startHeartbeat(QVariantMap);

//signals:
  //void filteredLog( QtMsgType type, const QMessageLogContext& context, const QString& msg);
private:
  std::shared_ptr<websocket_threaded_connection> m_ws;
  std::shared_ptr<spdlog::logger> m_logger;
  std::shared_ptr<websocket_heartbeat> m_heartbeat;

  QString m_appName;
  QString m_appVersion;
  QString m_appCreator;
  QString m_loggerHost;
  QStringList m_logFilter;

  quint32 m_heartbeatDur{5};
  log_level m_logLevel{Debug};
  bool m_logQtMessages{};
};
}
}
