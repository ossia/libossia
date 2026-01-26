#pragma once
#include <ossia/network/context.hpp>

#include <QObject>

#if QT_VERSION > QT_VERSION_CHECK(6, 10, 0)
#if QT_NETWORKAUTH_LIB
#include <QJSValue>
#include <QQmlEngine>
#include <QUrl>
#include <QVariant>

#include <verdigris>
#include <nano_observer.hpp>

class QNetworkAccessManager;
class QOAuthHttpServerReplyHandler;
class QOAuth2AuthorizationCodeFlow;
namespace ossia::qt
{
class OSSIA_EXPORT qml_oauth
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_oauth)
public:
  qml_oauth();
  ~qml_oauth();

  void grant();
  W_INVOKABLE(grant)

  void granted() E_SIGNAL(OSSIA_EXPORT, granted);

  QString token() const noexcept;
  void setToken(const QString& tk) noexcept;
  void tokenChanged(QString token) E_SIGNAL(OSSIA_EXPORT, tokenChanged, token);

  QString clientIdentifier() const noexcept;
  void setClientIdentifier(const QString& tk) noexcept;
  void clientIdentifierChanged(QString token)
      E_SIGNAL(OSSIA_EXPORT, clientIdentifierChanged, token);

  QString clientIdentifierSharedKey() const noexcept;
  void setClientIdentifierSharedKey(const QString& tk) noexcept;
  void clientIdentifierSharedKeyChanged(QString token)
      E_SIGNAL(OSSIA_EXPORT, clientIdentifierSharedKeyChanged, token);

  QUrl authorizationUrl() const noexcept;
  void setAuthorizationUrl(const QUrl& tk) noexcept;
  void authorizationUrlChanged(QUrl token)
      E_SIGNAL(OSSIA_EXPORT, authorizationUrlChanged, token);

  QUrl accessTokenUrl() const noexcept;
  void setAccessTokenUrl(const QUrl& tk) noexcept;
  void accessTokenUrlChanged(QUrl token)
      E_SIGNAL(OSSIA_EXPORT, accessTokenUrlChanged, token);

  QStringList requestedScopeTokens() const noexcept;
  void setRequestedScopeTokens(const QStringList& tk) noexcept;
  void requestedScopeTokensChanged(QStringList token)
      E_SIGNAL(OSSIA_EXPORT, requestedScopeTokensChanged, token);

  void requestFailed(QString error) E_SIGNAL(OSSIA_EXPORT, requestFailed, error);
  void send(QString verb, QUrl url, QJSValue config, QJSValue callback);
  W_INVOKABLE(send)

  W_PROPERTY(QString, accessToken READ token WRITE setToken NOTIFY tokenChanged)
  W_PROPERTY(QUrl, accessTokenUrl READ accessTokenUrl WRITE setAccessTokenUrl NOTIFY accessTokenUrlChanged)
  W_PROPERTY(QUrl, authorizationUrl READ authorizationUrl WRITE setAuthorizationUrl NOTIFY authorizationUrlChanged)
  W_PROPERTY(QString, clientIdentifierSharedKey READ clientIdentifierSharedKey WRITE setClientIdentifierSharedKey NOTIFY clientIdentifierSharedKeyChanged)
  W_PROPERTY(QString, clientIdentifier READ clientIdentifier WRITE setClientIdentifier NOTIFY clientIdentifierChanged)
  W_PROPERTY(QStringList, requestedScopeTokens READ requestedScopeTokens WRITE setRequestedScopeTokens NOTIFY requestedScopeTokensChanged)

  // Callbacks
  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onMessage;

private:
  QNetworkAccessManager* m_networkManager{};
  QOAuth2AuthorizationCodeFlow* m_oauth{};
  QOAuthHttpServerReplyHandler* m_replyHandler{};
};

}

Q_DECLARE_METATYPE(ossia::qt::qml_oauth*)
W_REGISTER_ARGTYPE(ossia::qt::qml_oauth*)
#endif
#endif
