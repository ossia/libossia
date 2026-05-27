#pragma once

#if __has_include(<QNearFieldManager>)
#define OSSIA_HAS_NFC 1

#include <ossia-qt/protocols/utils.hpp>

#include <QJSValue>
#include <QNdefMessage>
#include <QNdefNfcTextRecord>
#include <QNdefNfcUriRecord>
#include <QNdefRecord>
#include <QNearFieldManager>
#include <QNearFieldTarget>
#include <QObject>
#include <QPointer>
#include <QQmlEngine>
#include <QVariant>

#include <nano_observer.hpp>

#include <verdigris>

namespace ossia::qt
{

class qml_nfc_target
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_nfc_target)
public:
  explicit qml_nfc_target(QNearFieldTarget* target, QObject* parent = nullptr)
      : QObject{parent}
      , m_target{target}
  {
    QObject::connect(
        m_target, &QNearFieldTarget::ndefMessageRead, this,
        [this](const QNdefMessage& message) {
          if(!onNdefMessage.isCallable())
            return;
          auto* engine = qjsEngine(this);
          if(!engine)
            return;

          auto arr = engine->newArray(message.size());
          for(qsizetype i = 0; i < message.size(); ++i)
          {
            const auto& record = message[i];
            auto obj = engine->newObject();

            if(record.isRecordType<QNdefNfcTextRecord>())
            {
              QNdefNfcTextRecord textRecord(record);
              obj.setProperty("type", QStringLiteral("text"));
              obj.setProperty("text", textRecord.text());
              obj.setProperty("locale", textRecord.locale());
              obj.setProperty(
                  "encoding",
                  textRecord.encoding() == QNdefNfcTextRecord::Utf8
                      ? QStringLiteral("UTF-8")
                      : QStringLiteral("UTF-16"));
            }
            else if(record.isRecordType<QNdefNfcUriRecord>())
            {
              QNdefNfcUriRecord uriRecord(record);
              obj.setProperty("type", QStringLiteral("uri"));
              obj.setProperty("uri", uriRecord.uri().toString());
            }
            else
            {
              obj.setProperty("type", QStringLiteral("raw"));
              obj.setProperty(
                  "typeNameFormat",
                  static_cast<int>(record.typeNameFormat()));
              obj.setProperty(
                  "recordType", engine->toScriptValue(record.type()));
              obj.setProperty(
                  "payload", engine->toScriptValue(record.payload()));
              obj.setProperty("id", engine->toScriptValue(record.id()));
            }

            arr.setProperty(i, obj);
          }
          onNdefMessage.call({arr});
        });

    QObject::connect(
        m_target, &QNearFieldTarget::error, this,
        [this](
            QNearFieldTarget::Error error,
            const QNearFieldTarget::RequestId&) {
          if(!onError.isCallable())
            return;
          QString msg;
          switch(error)
          {
            case QNearFieldTarget::NoError:
              return;
            case QNearFieldTarget::UnsupportedError:
              msg = QStringLiteral("Unsupported operation");
              break;
            case QNearFieldTarget::TargetOutOfRangeError:
              msg = QStringLiteral("Target out of range");
              break;
            case QNearFieldTarget::NoResponseError:
              msg = QStringLiteral("No response");
              break;
            case QNearFieldTarget::ChecksumMismatchError:
              msg = QStringLiteral("Checksum mismatch");
              break;
            case QNearFieldTarget::InvalidParametersError:
              msg = QStringLiteral("Invalid parameters");
              break;
            case QNearFieldTarget::ConnectionError:
              msg = QStringLiteral("Connection error");
              break;
            case QNearFieldTarget::NdefReadError:
              msg = QStringLiteral("NDEF read error");
              break;
            case QNearFieldTarget::NdefWriteError:
              msg = QStringLiteral("NDEF write error");
              break;
            case QNearFieldTarget::CommandError:
              msg = QStringLiteral("Command error");
              break;
            case QNearFieldTarget::TimeoutError:
              msg = QStringLiteral("Timeout");
              break;
            default:
              msg = QStringLiteral("Unknown NFC error");
              break;
          }
          onError.call({msg});
        });
  }

  QString uid() const
  {
    if(!m_target)
      return {};
    return m_target->uid().toHex(':');
  }
  W_SLOT(uid)

  int type() const
  {
    if(!m_target)
      return -1;
    return static_cast<int>(m_target->type());
  }
  W_SLOT(type)

  void readNdef()
  {
    if(m_target)
      m_target->readNdefMessages();
  }
  W_SLOT(readNdef)

  void writeNdef(QJSValue records)
  {
    if(!m_target)
      return;
    auto* engine = qjsEngine(this);
    if(!engine || !records.isArray())
      return;

    QNdefMessage message;
    auto length = records.property("length").toInt();
    for(int i = 0; i < length; ++i)
    {
      auto rec = records.property(i);
      auto recType = rec.property("type").toString();
      if(recType == "text")
      {
        QNdefNfcTextRecord textRecord;
        textRecord.setText(rec.property("text").toString());
        if(rec.hasProperty("locale"))
          textRecord.setLocale(rec.property("locale").toString());
        else
          textRecord.setLocale(QStringLiteral("en"));
        message.append(textRecord);
      }
      else if(recType == "uri")
      {
        QNdefNfcUriRecord uriRecord;
        uriRecord.setUri(QUrl(rec.property("uri").toString()));
        message.append(uriRecord);
      }
      else if(recType == "raw")
      {
        QNdefRecord rawRecord;
        rawRecord.setTypeNameFormat(static_cast<QNdefRecord::TypeNameFormat>(
            rec.property("typeNameFormat").toInt()));
        rawRecord.setType(rec.property("recordType").toVariant().toByteArray());
        rawRecord.setPayload(
            rec.property("payload").toVariant().toByteArray());
        if(rec.hasProperty("id"))
          rawRecord.setId(rec.property("id").toVariant().toByteArray());
        message.append(rawRecord);
      }
    }

    m_target->writeNdefMessages({message});
  }
  W_SLOT(writeNdef)

  void sendCommand(QByteArray command)
  {
    if(m_target)
      m_target->sendCommand(command);
  }
  W_SLOT(sendCommand)

  QJSValue onNdefMessage;
  QJSValue onError;

private:
  QPointer<QNearFieldTarget> m_target;
};

class qml_nfc_scanner
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_nfc_scanner)
public:
  qml_nfc_scanner()
  {
    m_manager = new QNearFieldManager(this);

    QObject::connect(
        m_manager, &QNearFieldManager::targetDetected, this,
        [this](QNearFieldTarget* target) {
          if(!onTargetDetected.isCallable())
            return;
          auto* engine = qjsEngine(this);
          if(!engine)
            return;

          // Qt's QNearFieldManager retains ownership of the target for the
          // manager's lifetime (per docs). On some backends (Android) the
          // same target pointer can be re-emitted via targetDetected when
          // a previously seen tag re-enters range. Reuse the existing
          // wrapper in that case so we don't end up with two wrappers
          // connected to the same target's signals (which would double
          // up onNdefMessage/onError callbacks).
          auto existing = target->findChildren<qml_nfc_target*>(
              QString(), Qt::FindDirectChildrenOnly);
          if(!existing.isEmpty())
          {
            onTargetDetected.call({engine->newQObject(existing.first())});
            return;
          }

          // Parent the wrapper to the target so it lives as long as the
          // target itself (which is owned by m_manager). Otherwise the
          // wrapper would be JS-owned and could be garbage-collected
          // while asynchronous NFC operations are still in flight.
          auto* wrapper = new qml_nfc_target(target, target);
          auto jsWrapper = engine->newQObject(wrapper);

          if(onNdefMessage.isCallable())
            wrapper->onNdefMessage = onNdefMessage;
          if(onError.isCallable())
            wrapper->onError = onError;

          onTargetDetected.call({jsWrapper});
        });

    QObject::connect(
        m_manager, &QNearFieldManager::targetLost, this,
        [this](QNearFieldTarget*) {
          if(onTargetLost.isCallable())
            onTargetLost.call();
        });

    QObject::connect(
        m_manager, &QNearFieldManager::adapterStateChanged, this,
        [this](QNearFieldManager::AdapterState state) {
          if(!onAdapterStateChanged.isCallable())
            return;
          QString stateStr;
          switch(state)
          {
            case QNearFieldManager::AdapterState::Offline:
              stateStr = QStringLiteral("offline");
              break;
            case QNearFieldManager::AdapterState::TurningOn:
              stateStr = QStringLiteral("turning_on");
              break;
            case QNearFieldManager::AdapterState::Online:
              stateStr = QStringLiteral("online");
              break;
            case QNearFieldManager::AdapterState::TurningOff:
              stateStr = QStringLiteral("turning_off");
              break;
          }
          onAdapterStateChanged.call({stateStr});
        });
  }

  ~qml_nfc_scanner() { stop(); }

  void start()
  {
    m_manager->startTargetDetection(QNearFieldTarget::NdefAccess);
  }
  W_SLOT(start)

  void startRaw()
  {
    m_manager->startTargetDetection(QNearFieldTarget::TagTypeSpecificAccess);
  }
  W_SLOT(startRaw)

  void stop() { m_manager->stopTargetDetection(); }
  W_SLOT(stop)

  bool isEnabled() const { return m_manager->isEnabled(); }
  W_SLOT(isEnabled)

  QJSValue onTargetDetected;
  QJSValue onTargetLost;
  QJSValue onNdefMessage;
  QJSValue onAdapterStateChanged;
  QJSValue onError;

private:
  QNearFieldManager* m_manager{};
};

} // namespace ossia::qt

#endif // __has_include(<QNearFieldManager>)
