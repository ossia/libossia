#pragma once
#include <QObject>
#include <QString>
#include <ossia_export.h>
#include <memory>
namespace spdlog { class logger; }

namespace ossia
{
class websocket_threaded_connection;
class websocket_heartbeat;
namespace qt
{
class OSSIA_EXPORT qml_logger:
    public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString appName READ appName WRITE setAppName NOTIFY appNameChanged FINAL)
    Q_PROPERTY(QString appVersion READ appVersion WRITE setAppVersion NOTIFY appVersionChanged FINAL)
    Q_PROPERTY(QString appCreator READ appCreator WRITE setAppCreator NOTIFY appCreatorChanged FINAL)
    Q_PROPERTY(QString loggerHost READ loggerHost WRITE setLoggerHost NOTIFY loggerHostChanged)

  public:
    qml_logger();
    QString appName() const;
    QString appVersion() const;
    QString appCreator() const;

    QString loggerHost() const;

  signals:
    void appNameChanged(QString appName);
    void appVersionChanged(QString appVersion);
    void appCreatorChanged(QString appCreator);
    void loggerHostChanged(QString loggerHost);

  public slots:
    void setAppName(QString appName);
    void setAppVersion(QString appVersion);
    void setAppCreator(QString appCreator);

    void logTrace(const QString& s);
    void logInfo(const QString& s);
    void logDebug(const QString& s);
    void logWarning(const QString& s);
    void logError(const QString& s);
    void logCritical(const QString& s);
    void setLoggerHost(QString loggerHost);

  private:
    void connectLogger();
    std::shared_ptr<websocket_threaded_connection> m_ws;
    std::shared_ptr<spdlog::logger> m_logger;
    std::shared_ptr<websocket_heartbeat> m_heartbeat;

    QString m_appName;
    QString m_appVersion;
    QString m_appCreator;
    QString m_loggerHost;

};
}
}
