#include "qml_oauth.hpp"

#if QT_VERSION > QT_VERSION_CHECK(6, 10, 0)
#if QT_NETWORKAUTH_LIB

#include <ossia-qt/protocols/utils.hpp>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#if QT_GUI_LIB
#include <QDesktopServices>
#endif
#include <QHttpMultiPart>

namespace ossia::qt
{

qml_oauth::qml_oauth()
{
  m_networkManager = new QNetworkAccessManager{this};
  m_oauth = new QOAuth2AuthorizationCodeFlow{m_networkManager, this};
  connect(
      m_oauth, &QOAuth2AuthorizationCodeFlow::clientIdentifierChanged, this,
      &qml_oauth::clientIdentifierChanged);
  connect(
      m_oauth, &QOAuth2AuthorizationCodeFlow::clientIdentifierSharedKeyChanged, this,
      &qml_oauth::clientIdentifierSharedKeyChanged);
  connect(
      m_oauth, &QOAuth2AuthorizationCodeFlow::authorizationUrlChanged, this,
      &qml_oauth::authorizationUrlChanged);
  connect(
      m_oauth, &QOAuth2AuthorizationCodeFlow::accessTokenUrlChanged, this,
      &qml_oauth::accessTokenUrlChanged);
  connect(
      m_oauth, &QOAuth2AuthorizationCodeFlow::tokenChanged, this,
      &qml_oauth::tokenChanged);
  connect(m_oauth, &QOAuth2AuthorizationCodeFlow::granted, this, &qml_oauth::granted);

#if QT_GUI_LIB
  connect(
      m_oauth, &QAbstractOAuth::authorizeWithBrowser, this, &QDesktopServices::openUrl);
#endif

  m_replyHandler = new QOAuthHttpServerReplyHandler{this};
  m_oauth->setReplyHandler(m_replyHandler);

  connect(
      m_oauth, &QOAuth2AuthorizationCodeFlow::requestFailed,
      [this](QOAuth2AuthorizationCodeFlow::Error e) {
    switch(e)
    {
      case QOAuth2AuthorizationCodeFlow::Error::NoError:
        requestFailed("NoError");
        break;
      case QOAuth2AuthorizationCodeFlow::Error::NetworkError:
        requestFailed("NetworkError");
        break;
      case QOAuth2AuthorizationCodeFlow::Error::ServerError:
        requestFailed("ServerError");
        break;
      case QOAuth2AuthorizationCodeFlow::Error::OAuthTokenNotFoundError:
        requestFailed("OAuthTokenNotFoundError");
        break;
      case QOAuth2AuthorizationCodeFlow::Error::OAuthTokenSecretNotFoundError:
        requestFailed("OAuthTokenSecretNotFoundError");
        break;
      case QOAuth2AuthorizationCodeFlow::Error::OAuthCallbackNotVerified:
        requestFailed("OAuthCallbackNotVerified");
        break;
      case QOAuth2AuthorizationCodeFlow::Error::ClientError:
        requestFailed("ClientError");
        break;
      case QOAuth2AuthorizationCodeFlow::Error::ExpiredError:
        requestFailed("ExpiredError");
        break;
    }
  });
}

qml_oauth::~qml_oauth() { }

void qml_oauth::grant()
{
  m_oauth->grant();
}

QString qml_oauth::token() const noexcept
{
  return m_oauth->token();
}

void qml_oauth::setToken(const QString& tk) noexcept
{
  m_oauth->setToken(tk);
}

QString qml_oauth::clientIdentifier() const noexcept
{
  return m_oauth->clientIdentifier();
}

void qml_oauth::setClientIdentifier(const QString& tk) noexcept
{
  m_oauth->setClientIdentifier(tk);
}

QString qml_oauth::clientIdentifierSharedKey() const noexcept
{
  return m_oauth->clientIdentifierSharedKey();
}

void qml_oauth::setClientIdentifierSharedKey(const QString& tk) noexcept
{
  m_oauth->setClientIdentifierSharedKey(tk);
}

QUrl qml_oauth::authorizationUrl() const noexcept
{
  return m_oauth->authorizationUrl();
}

void qml_oauth::setAuthorizationUrl(const QUrl& tk) noexcept
{
  m_oauth->setAuthorizationUrl(tk);
}

QUrl qml_oauth::accessTokenUrl() const noexcept
{
  return m_oauth->accessTokenUrl();
}

void qml_oauth::setAccessTokenUrl(const QUrl& tk) noexcept
{
  m_oauth->setAccessTokenUrl(tk);
}

QStringList qml_oauth::requestedScopeTokens() const noexcept
{
  QStringList ret;
  auto tok = m_oauth->requestedScopeTokens();
  for(auto& t : tok)
  {
    ret.push_back(QString::fromUtf8(t));
  }
  return ret;
}

void qml_oauth::setRequestedScopeTokens(const QStringList& tk) noexcept
{
  QSet<QByteArray> ba;
  for(auto& t : tk)
  {
    ba.insert(t.toUtf8());
  }
  if(ba != m_oauth->requestedScopeTokens())
  {
    m_oauth->setRequestedScopeTokens(ba);
    requestedScopeTokensChanged(tk);
  }
}

void qml_oauth::send(QString verb, QUrl url, QJSValue config, QJSValue callback)
{
  QNetworkRequest request(url);
  QByteArray verbBytes = verb.toUtf8();

  // 1. Set Custom Headers from JS Config
  if(config.hasProperty("headers"))
  {
    QJSValue headers = config.property("headers");
    QJSValueIterator it(headers);
    while(it.next())
    {
      request.setRawHeader(it.name().toLatin1(), it.value().toString().toUtf8());
    }
  }

  // 2. Sign the Request (Adds Authorization: Bearer ...)
  m_oauth->prepareRequest(&request, verbBytes);

  // 3. Handle Body (None, Raw String, or Multipart)
  QJSValue body = config.property("body");
  QNetworkReply* reply = nullptr;

  if(body.isObject() && body.hasProperty("parts"))
  {
    // --- Multipart Upload ---
    QHttpMultiPart::ContentType type = QHttpMultiPart::MixedType;
    QString typeStr = body.property("type").toString();
    if(typeStr == "related")
      type = QHttpMultiPart::RelatedType;
    else if(typeStr == "form-data")
      type = QHttpMultiPart::FormDataType;

    QHttpMultiPart* multiPart = new QHttpMultiPart(type);

    QJSValue parts = body.property("parts");
    int len = parts.property("length").toInt();
    for(int i = 0; i < len; ++i)
    {
      QJSValue jsPart = parts.property(i);
      QHttpPart httpPart;

      // Part Headers
      if(jsPart.hasProperty("headers"))
      {
        QJSValue ph = jsPart.property("headers");
        QJSValueIterator pit(ph);
        while(pit.next())
        {
          httpPart.setRawHeader(pit.name().toLatin1(), pit.value().toString().toUtf8());
        }
      }

      // Part Body: File or Data
      if(jsPart.hasProperty("file"))
      {
        QString path = jsPart.property("file").toString();
        if(path.startsWith("file://"))
          path = QUrl(path).toLocalFile();

        QFile* file = new QFile(path);
        if(file->open(QIODevice::ReadOnly))
        {
          httpPart.setBodyDevice(file);
          file->setParent(multiPart); // File dies with MultiPart
        }
        else
        {
          qDebug() << "qml_oauth: Failed to open file" << path;
          delete file;
        }
      }
      else if(jsPart.hasProperty("data"))
      {
        httpPart.setBody(jsPart.property("data").toString().toUtf8());
      }

      multiPart->append(httpPart);
    }

    reply = m_networkManager->sendCustomRequest(request, verbBytes, multiPart);
    multiPart->setParent(reply); // MultiPart dies with Reply
  }
  else if(body.isString())
  {
    // --- Raw String Body (JSON, XML, etc.) ---
    QByteArray data = body.toString().toUtf8();
    reply = m_networkManager->sendCustomRequest(request, verbBytes, data);
  }
  else
  {
    // --- No Body (GET, DELETE, etc.) ---
    reply = m_networkManager->sendCustomRequest(request, verbBytes);
  }

  // 4. Handle Response asynchronously
  if(reply)
  {
    connect(reply, &QNetworkReply::finished, this, [reply, callback, this]() mutable {
      QJSValue result;
      QJSValue error = QJSValue::UndefinedValue;
      auto engine = qjsEngine(this);

      if(reply->error() == QNetworkReply::NoError)
      {
        // Try parsing JSON, fallback to string
        QString respStr = QString::fromUtf8(reply->readAll());
        QJSValue parsed = engine->evaluate("(" + respStr + ")");
        result = parsed.isError() ? QJSValue(respStr) : parsed;
      }
      else
      {
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
      if(callback.isCallable())
        callback.call(args);

      reply->deleteLater();
    });
  }
}
}
#endif
#endif
