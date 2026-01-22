#pragma once
#include <ossia/network/context.hpp>

#include <ossia-qt/protocols/utils.hpp>

#if QT_VERSION > QT_VERSION_CHECK(6, 10, 0)
#if QT_NETWORKAUTH_LIB
#include <QJSValue>
#include <QObject>
#include <QQmlEngine>
#include <QVariant>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#if QT_GUI_LIB
#include <QDesktopServices>
#endif
#include <nano_observer.hpp>

#include <QHttpMultiPart>
#include <verdigris>

namespace ossia::qt
{
class qml_oauth
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_oauth)
public:
  qml_oauth() {
    m_networkManager = new QNetworkAccessManager{this};
    m_oauth = new QOAuth2AuthorizationCodeFlow{m_networkManager, this};
    connect(m_oauth, &QOAuth2AuthorizationCodeFlow::clientIdentifierChanged,
            this, &qml_oauth::clientIdentifierChanged);
    connect(m_oauth, &QOAuth2AuthorizationCodeFlow::clientIdentifierSharedKeyChanged,
            this, &qml_oauth::clientIdentifierSharedKeyChanged);
    connect(m_oauth, &QOAuth2AuthorizationCodeFlow::authorizationUrlChanged,
            this, &qml_oauth::authorizationUrlChanged);
    connect(m_oauth, &QOAuth2AuthorizationCodeFlow::accessTokenUrlChanged,
            this, &qml_oauth::accessTokenUrlChanged);
    connect(m_oauth, &QOAuth2AuthorizationCodeFlow::tokenChanged,
            this, &qml_oauth::tokenChanged);
    connect(m_oauth, &QOAuth2AuthorizationCodeFlow::granted,
            this, &qml_oauth::granted);

#if QT_GUI_LIB
    connect(m_oauth, &QAbstractOAuth::authorizeWithBrowser,
            this, &QDesktopServices::openUrl);
#endif

    m_replyHandler = new QOAuthHttpServerReplyHandler{this};
    m_oauth->setReplyHandler(m_replyHandler);

    connect(m_oauth, &QOAuth2AuthorizationCodeFlow::requestFailed,
            [this] (QOAuth2AuthorizationCodeFlow::Error e) {
      switch(e)
      {
        case QOAuth2AuthorizationCodeFlow::Error::NoError: requestFailed("NoError"); break;
        case QOAuth2AuthorizationCodeFlow::Error::NetworkError: requestFailed("NetworkError"); break;
        case QOAuth2AuthorizationCodeFlow::Error::ServerError: requestFailed("ServerError"); break;
        case QOAuth2AuthorizationCodeFlow::Error::OAuthTokenNotFoundError: requestFailed("OAuthTokenNotFoundError"); break;
        case QOAuth2AuthorizationCodeFlow::Error::OAuthTokenSecretNotFoundError: requestFailed("OAuthTokenSecretNotFoundError"); break;
        case QOAuth2AuthorizationCodeFlow::Error::OAuthCallbackNotVerified: requestFailed("OAuthCallbackNotVerified"); break;
        case QOAuth2AuthorizationCodeFlow::Error::ClientError: requestFailed("ClientError"); break;
        case QOAuth2AuthorizationCodeFlow::Error::ExpiredError: requestFailed("ExpiredError"); break;
      }
    });
  }

  ~qml_oauth() {}

  void grant()
  {
    m_oauth->grant();
  }
  W_INVOKABLE(grant)

  void granted() E_SIGNAL(OSSIA_EXPORT, granted);

  QString token() const noexcept
  { return m_oauth->token(); }
  void setToken(const QString& tk) noexcept
  {
    m_oauth->setToken(tk);
  }
  void tokenChanged(QString token)
  E_SIGNAL(OSSIA_EXPORT, tokenChanged, token);

  QString clientIdentifier() const noexcept
  { return m_oauth->clientIdentifier(); }
  void setClientIdentifier(const QString& tk) noexcept
  {
    m_oauth->setClientIdentifier(tk);
  }
  void clientIdentifierChanged(QString token)
      E_SIGNAL(OSSIA_EXPORT, clientIdentifierChanged, token);

  QString clientIdentifierSharedKey() const noexcept
  { return m_oauth->clientIdentifierSharedKey(); }
  void setClientIdentifierSharedKey(const QString& tk) noexcept
  {
    m_oauth->setClientIdentifierSharedKey(tk);
  }
  void clientIdentifierSharedKeyChanged(QString token)
      E_SIGNAL(OSSIA_EXPORT, clientIdentifierSharedKeyChanged, token);

  QUrl authorizationUrl() const noexcept
  { return m_oauth->authorizationUrl(); }
  void setAuthorizationUrl(const QUrl& tk) noexcept
  {
    m_oauth->setAuthorizationUrl(tk);
  }
  void authorizationUrlChanged(QUrl token)
      E_SIGNAL(OSSIA_EXPORT, authorizationUrlChanged, token);

  QUrl accessTokenUrl() const noexcept
  { return m_oauth->accessTokenUrl(); }
  void setAccessTokenUrl(const QUrl& tk) noexcept
  {
    m_oauth->setAccessTokenUrl(tk);
  }
  void accessTokenUrlChanged(QUrl token)
      E_SIGNAL(OSSIA_EXPORT, accessTokenUrlChanged, token);

  QStringList requestedScopeTokens() const noexcept
  {
    QStringList ret;
    auto tok = m_oauth->requestedScopeTokens();
    for(auto& t : tok) {
      ret.push_back(QString::fromUtf8(t));
    }
    return ret;
  }
  void setRequestedScopeTokens(const QStringList& tk) noexcept
  {
    QSet<QByteArray> ba;
    for(auto& t : tk) {
      ba.insert(t.toUtf8());
    }
    if(ba != m_oauth->requestedScopeTokens()) {
      m_oauth->setRequestedScopeTokens(ba);
      requestedScopeTokensChanged(tk);
    }
  }
  void requestedScopeTokensChanged(QStringList token)
      E_SIGNAL(OSSIA_EXPORT, requestedScopeTokensChanged, token);

  void requestFailed(QString error)
      E_SIGNAL(OSSIA_EXPORT, requestFailed, error);
  void send(QString verb, QUrl url, QJSValue config, QJSValue callback)
  {
    QNetworkRequest request(url);
    QByteArray verbBytes = verb.toUtf8();

    // 1. Set Custom Headers from JS Config
    if (config.hasProperty("headers")) {
      QJSValue headers = config.property("headers");
      QJSValueIterator it(headers);
      while (it.next()) {
        request.setRawHeader(it.name().toLatin1(), it.value().toString().toUtf8());
      }
    }

    // 2. Sign the Request (Adds Authorization: Bearer ...)
    m_oauth->prepareRequest(&request, verbBytes);

    // 3. Handle Body (None, Raw String, or Multipart)
    QJSValue body = config.property("body");
    QNetworkReply* reply = nullptr;

    if (body.isObject() && body.hasProperty("parts")) {
      // --- Multipart Upload ---
      QHttpMultiPart::ContentType type = QHttpMultiPart::MixedType;
      QString typeStr = body.property("type").toString();
      if(typeStr == "related") type = QHttpMultiPart::RelatedType;
      else if(typeStr == "form-data") type = QHttpMultiPart::FormDataType;

      QHttpMultiPart* multiPart = new QHttpMultiPart(type);

      QJSValue parts = body.property("parts");
      int len = parts.property("length").toInt();
      for(int i = 0; i < len; ++i) {
        QJSValue jsPart = parts.property(i);
        QHttpPart httpPart;

        // Part Headers
        if(jsPart.hasProperty("headers")) {
          QJSValue ph = jsPart.property("headers");
          QJSValueIterator pit(ph);
          while (pit.next()) {
            httpPart.setRawHeader(pit.name().toLatin1(), pit.value().toString().toUtf8());
          }
        }

        // Part Body: File or Data
        if (jsPart.hasProperty("file")) {
          QString path = jsPart.property("file").toString();
          if(path.startsWith("file://")) path = QUrl(path).toLocalFile();

          QFile* file = new QFile(path);
          if (file->open(QIODevice::ReadOnly)) {
            httpPart.setBodyDevice(file);
            file->setParent(multiPart); // File dies with MultiPart
          } else {
            qDebug() << "qml_oauth: Failed to open file" << path;
            delete file;
          }
        } else if (jsPart.hasProperty("data")) {
          httpPart.setBody(jsPart.property("data").toString().toUtf8());
        }

        multiPart->append(httpPart);
      }

      reply = m_networkManager->sendCustomRequest(request, verbBytes, multiPart);
      multiPart->setParent(reply); // MultiPart dies with Reply

    } else if (body.isString()) {
      // --- Raw String Body (JSON, XML, etc.) ---
      QByteArray data = body.toString().toUtf8();
      reply = m_networkManager->sendCustomRequest(request, verbBytes, data);
    } else {
      // --- No Body (GET, DELETE, etc.) ---
      reply = m_networkManager->sendCustomRequest(request, verbBytes);
    }

    // 4. Handle Response asynchronously
    if (reply) {
      connect(reply, &QNetworkReply::finished, this, [reply, callback, this]() mutable {
        QJSValue result;
        QJSValue error = QJSValue::UndefinedValue;
        auto engine = qjsEngine(this);

        if (reply->error() == QNetworkReply::NoError) {
          // Try parsing JSON, fallback to string
          QString respStr = QString::fromUtf8(reply->readAll());
          QJSValue parsed = engine->evaluate("(" + respStr + ")");
          result = parsed.isError() ? QJSValue(respStr) : parsed;
        } else {
          // Create Error Object
          QJSValue errObj = engine->newObject();
          errObj.setProperty("code", reply->error());
          errObj.setProperty("message", reply->errorString());
          errObj.setProperty("body", QString::fromUtf8(reply->readAll()));
          error = errObj;
        }

        // Call callback(result, error)
        QJSValueList args;
        args << result << error;
        if(callback.isCallable()) callback.call(args);

        reply->deleteLater();
      });
    }
  }
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
