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
    Q_PROPERTY(quint32 heartbeat READ heartbeat WRITE setHeartbeat NOTIFY heartbeatChanged)
    Q_PROPERTY(bool logQtMessages READ logQtMessages WRITE setLogQtMessages NOTIFY logQtMessagesChanged)

public:
    qml_logger();
    ~qml_logger();

    static qml_logger& instance();
    QString appName() const;
    QString appVersion() const;
    QString appCreator() const;
    QString loggerHost() const;

    bool logQtMessages() const;

    quint32 heartbeat() const;

signals:
    void appNameChanged(QString appName);
    void appVersionChanged(QString appVersion);
    void appCreatorChanged(QString appCreator);
    void loggerHostChanged(QString loggerHost);

    void logQtMessagesChanged(bool logQtMessages);

    void heartbeatChanged(quint32 heartbeat);

public slots:
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

    void setHeartbeat(quint32 heartbeat);

private:
    void connectLogger();
    std::shared_ptr<websocket_threaded_connection> m_ws;
    std::shared_ptr<spdlog::logger> m_logger;
    std::shared_ptr<websocket_heartbeat> m_heartbeat;

    QString m_appName;
    QString m_appVersion;
    QString m_appCreator;
    QString m_loggerHost;

    quint32 m_heartbeatDur{5};
    bool m_logQtMessages{};
};
}
}
