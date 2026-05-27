#pragma once

#if __has_include(<QBluetoothDeviceDiscoveryAgent>)
#define OSSIA_HAS_BLUETOOTH 1

#include <ossia-qt/protocols/utils.hpp>

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothSocket>
#include <QBluetoothUuid>
#include <QJSValue>
#include <QObject>
#include <QPointer>
#include <QQmlEngine>
#include <QVariant>
#include <QLowEnergyCharacteristic>
#include <QLowEnergyController>
#include <QLowEnergyDescriptor>
#include <QLowEnergyService>

#include <nano_observer.hpp>

#include <verdigris>

namespace ossia::qt
{

static QBluetoothUuid bleUuidFromString(const QString& str)
{
  if(str.length() <= 4)
  {
    bool ok = false;
    auto val = str.toUInt(&ok, 16);
    if(ok)
      return QBluetoothUuid{static_cast<quint16>(val)};
  }
  return QBluetoothUuid{QUuid::fromString(str)};
}

class qml_bluetooth_scanner
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_bluetooth_scanner)
public:
  qml_bluetooth_scanner()
  {
    m_agent = new QBluetoothDeviceDiscoveryAgent(this);

    QObject::connect(
        m_agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this,
        [this](const QBluetoothDeviceInfo& info) {
          if(!onDeviceDiscovered.isCallable())
            return;
          auto* engine = qjsEngine(this);
          if(!engine)
            return;

          auto dev = engine->newObject();
          dev.setProperty("Name", info.name());
          dev.setProperty("Address", info.address().toString());
          dev.setProperty(
              "DeviceUuid", info.deviceUuid().toString(QUuid::WithoutBraces));
          dev.setProperty("RSSI", info.rssi());
          dev.setProperty(
              "IsBLE",
              info.coreConfigurations().testFlag(
                  QBluetoothDeviceInfo::LowEnergyCoreConfiguration));
          dev.setProperty(
              "IsClassic",
              info.coreConfigurations().testFlag(
                  QBluetoothDeviceInfo::BaseRateCoreConfiguration));
          dev.setProperty(
              "MajorDeviceClass",
              static_cast<int>(info.majorDeviceClass()));
          dev.setProperty("MinorDeviceClass", info.minorDeviceClass());

          auto uuids = info.serviceUuids();
          auto serviceArr = engine->newArray(uuids.size());
          for(qsizetype i = 0; i < uuids.size(); ++i)
            serviceArr.setProperty(i, uuids[i].toString(QUuid::WithoutBraces));
          dev.setProperty("ServiceUuids", serviceArr);

          auto mfData = info.manufacturerData();
          if(!mfData.isEmpty())
          {
            auto mfObj = engine->newObject();
            for(auto it = mfData.begin(); it != mfData.end(); ++it)
              mfObj.setProperty(
                  QString::number(it.key()), engine->toScriptValue(it.value()));
            dev.setProperty("ManufacturerData", mfObj);
          }

          onDeviceDiscovered.call({dev});
        });

    QObject::connect(
        m_agent, &QBluetoothDeviceDiscoveryAgent::finished, this, [this]() {
          if(onFinished.isCallable())
            onFinished.call();
        });

    QObject::connect(
        m_agent, &QBluetoothDeviceDiscoveryAgent::errorOccurred, this,
        [this](QBluetoothDeviceDiscoveryAgent::Error) {
          if(onError.isCallable())
            onError.call({m_agent->errorString()});
        });
  }

  ~qml_bluetooth_scanner() { stop(); }

  void start()
  {
    m_agent->start(
        QBluetoothDeviceDiscoveryAgent::ClassicMethod
        | QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
  }
  W_SLOT(start)

  void startBLE()
  {
    m_agent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
  }
  W_SLOT(startBLE)

  void startClassic()
  {
    m_agent->start(QBluetoothDeviceDiscoveryAgent::ClassicMethod);
  }
  W_SLOT(startClassic)

  void stop() { m_agent->stop(); }
  W_SLOT(stop)

  void setLowEnergyDiscoveryTimeout(int msTimeout)
  {
    m_agent->setLowEnergyDiscoveryTimeout(msTimeout);
  }
  W_SLOT(setLowEnergyDiscoveryTimeout)

  QJSValue onDeviceDiscovered;
  QJSValue onFinished;
  QJSValue onError;

private:
  QBluetoothDeviceDiscoveryAgent* m_agent{};
};

class qml_bluetooth_socket
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_bluetooth_socket)
public:
  struct state
  {
    std::atomic_bool alive{true};
  };

  qml_bluetooth_socket(
      const QBluetoothAddress& address, const QBluetoothUuid& serviceUuid)
      : m_state{std::make_shared<state>()}
      , m_address{address}
      , m_serviceUuid{serviceUuid}
  {
    m_socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);

    auto st = m_state;
    QObject::connect(
        m_socket, &QBluetoothSocket::connected, this, [this, st]() {
          if(!st->alive)
            return;
          m_is_open = true;
          if(!onOpen.isCallable())
            return;
          auto* engine = qjsEngine(this);
          if(!engine)
            return;
          onOpen.call({engine->newQObject(this)});
        });

    QObject::connect(
        m_socket, &QBluetoothSocket::disconnected, this, [this, st]() {
          if(!st->alive)
            return;
          m_is_open = false;
          if(onClose.isCallable())
            onClose.call();
        });

    QObject::connect(
        m_socket, &QBluetoothSocket::readyRead, this, [this, st]() {
          if(!st->alive)
            return;
          if(!onMessage.isCallable())
            return;
          auto data = m_socket->readAll();
          auto* engine = qjsEngine(this);
          if(engine)
            onMessage.call({engine->toScriptValue(data)});
        });

    QObject::connect(
        m_socket, &QBluetoothSocket::errorOccurred, this,
        [this, st](QBluetoothSocket::SocketError) {
          if(!st->alive)
            return;
          if(onError.isCallable())
            onError.call({m_socket->errorString()});
        });
  }

  ~qml_bluetooth_socket()
  {
    m_state->alive = false;
    close();
  }

  void open() { m_socket->connectToService(m_address, m_serviceUuid); }

  void write(QByteArray data)
  {
    if(m_is_open)
      m_socket->write(data);
  }
  W_SLOT(write)

  void close()
  {
    if(m_is_open)
    {
      m_is_open = false;
      m_socket->disconnectFromService();
    }
  }
  W_SLOT(close)

  QJSValue onOpen;
  QJSValue onClose;
  QJSValue onError;
  QJSValue onMessage;

private:
  std::shared_ptr<state> m_state;
  QBluetoothSocket* m_socket{};
  QBluetoothAddress m_address;
  QBluetoothUuid m_serviceUuid;
  std::atomic_bool m_is_open{false};
};

class qml_ble_service
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_ble_service)
public:
  explicit qml_ble_service(QLowEnergyService* service, QObject* parent)
      : QObject{parent}
      , m_service{service}
  {
    QObject::connect(
        m_service, &QLowEnergyService::stateChanged, this,
        [this](QLowEnergyService::ServiceState state) {
          if(state != QLowEnergyService::RemoteServiceDiscovered)
            return;
          if(!onDetailsDiscovered.isCallable())
            return;
          auto* engine = qjsEngine(this);
          if(!engine)
            return;

          auto chars = m_service->characteristics();
          auto arr = engine->newArray(chars.size());
          for(qsizetype i = 0; i < chars.size(); ++i)
          {
            auto obj = engine->newObject();
            obj.setProperty(
                "uuid", chars[i].uuid().toString(QUuid::WithoutBraces));
            obj.setProperty("name", chars[i].name());
            obj.setProperty("value", engine->toScriptValue(chars[i].value()));
            obj.setProperty(
                "properties", static_cast<int>(chars[i].properties()));
            arr.setProperty(i, obj);
          }
          onDetailsDiscovered.call({arr});
        });

    QObject::connect(
        m_service, &QLowEnergyService::characteristicChanged, this,
        [this](const QLowEnergyCharacteristic& c, const QByteArray& value) {
          if(!onCharacteristicChanged.isCallable())
            return;
          auto* engine = qjsEngine(this);
          if(engine)
            onCharacteristicChanged.call(
                {c.uuid().toString(QUuid::WithoutBraces),
                 engine->toScriptValue(value)});
        });

    QObject::connect(
        m_service, &QLowEnergyService::characteristicRead, this,
        [this](const QLowEnergyCharacteristic& c, const QByteArray& value) {
          if(!onCharacteristicRead.isCallable())
            return;
          auto* engine = qjsEngine(this);
          if(engine)
            onCharacteristicRead.call(
                {c.uuid().toString(QUuid::WithoutBraces),
                 engine->toScriptValue(value)});
        });

    QObject::connect(
        m_service, &QLowEnergyService::characteristicWritten, this,
        [this](const QLowEnergyCharacteristic& c, const QByteArray& value) {
          if(!onCharacteristicWritten.isCallable())
            return;
          auto* engine = qjsEngine(this);
          if(engine)
            onCharacteristicWritten.call(
                {c.uuid().toString(QUuid::WithoutBraces),
                 engine->toScriptValue(value)});
        });

    QObject::connect(
        m_service, &QLowEnergyService::errorOccurred, this,
        [this](QLowEnergyService::ServiceError err) {
          if(!onError.isCallable())
            return;
          QString msg;
          switch(err)
          {
            case QLowEnergyService::NoError:
              return;
            case QLowEnergyService::OperationError:
              msg = QStringLiteral("Operation error");
              break;
            case QLowEnergyService::CharacteristicWriteError:
              msg = QStringLiteral("Characteristic write error");
              break;
            case QLowEnergyService::DescriptorWriteError:
              msg = QStringLiteral("Descriptor write error");
              break;
            case QLowEnergyService::CharacteristicReadError:
              msg = QStringLiteral("Characteristic read error");
              break;
            case QLowEnergyService::DescriptorReadError:
              msg = QStringLiteral("Descriptor read error");
              break;
            default:
              msg = QStringLiteral("Unknown service error");
              break;
          }
          onError.call({msg});
        });
  }

  QString uuid() const
  {
    return m_service->serviceUuid().toString(QUuid::WithoutBraces);
  }
  W_SLOT(uuid)

  void discoverDetails() { m_service->discoverDetails(); }
  W_SLOT(discoverDetails)

  QJSValue characteristics()
  {
    auto* engine = qjsEngine(this);
    if(!engine)
      return {};

    auto chars = m_service->characteristics();
    auto arr = engine->newArray(chars.size());
    for(qsizetype i = 0; i < chars.size(); ++i)
    {
      auto obj = engine->newObject();
      obj.setProperty("uuid", chars[i].uuid().toString(QUuid::WithoutBraces));
      obj.setProperty("name", chars[i].name());
      obj.setProperty("value", engine->toScriptValue(chars[i].value()));
      obj.setProperty("properties", static_cast<int>(chars[i].properties()));
      arr.setProperty(i, obj);
    }
    return arr;
  }
  W_SLOT(characteristics)

  void readCharacteristic(QString uuid)
  {
    auto c = m_service->characteristic(bleUuidFromString(uuid));
    if(c.isValid())
      m_service->readCharacteristic(c);
  }
  W_SLOT(readCharacteristic)

  void writeCharacteristic(QString uuid, QByteArray value)
  {
    auto c = m_service->characteristic(bleUuidFromString(uuid));
    if(c.isValid())
      m_service->writeCharacteristic(c, value);
  }
  W_SLOT(writeCharacteristic)

  void writeCharacteristicNoResponse(QString uuid, QByteArray value)
  {
    auto c = m_service->characteristic(bleUuidFromString(uuid));
    if(c.isValid())
      m_service->writeCharacteristic(
          c, value, QLowEnergyService::WriteWithoutResponse);
  }
  W_SLOT(writeCharacteristicNoResponse)

  void enableNotifications(QString uuid)
  {
    auto c = m_service->characteristic(bleUuidFromString(uuid));
    if(!c.isValid())
      return;
    auto cccd = c.clientCharacteristicConfiguration();
    if(!cccd.isValid())
      return;

    if(c.properties().testFlag(QLowEnergyCharacteristic::Indicate))
      m_service->writeDescriptor(
          cccd, QLowEnergyCharacteristic::CCCDEnableIndication);
    else
      m_service->writeDescriptor(
          cccd, QLowEnergyCharacteristic::CCCDEnableNotification);
  }
  W_SLOT(enableNotifications)

  void disableNotifications(QString uuid)
  {
    auto c = m_service->characteristic(bleUuidFromString(uuid));
    if(!c.isValid())
      return;
    auto cccd = c.clientCharacteristicConfiguration();
    if(cccd.isValid())
      m_service->writeDescriptor(cccd, QLowEnergyCharacteristic::CCCDDisable);
  }
  W_SLOT(disableNotifications)

  QJSValue onDetailsDiscovered;
  QJSValue onCharacteristicRead;
  QJSValue onCharacteristicWritten;
  QJSValue onCharacteristicChanged;
  QJSValue onError;

private:
  QLowEnergyService* m_service{};
};

class qml_ble_controller
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(qml_ble_controller)
public:
  struct state
  {
    std::atomic_bool alive{true};
  };

  explicit qml_ble_controller(const QBluetoothDeviceInfo& info)
      : m_state{std::make_shared<state>()}
      , m_deviceInfo{info}
  {
    m_controller = QLowEnergyController::createCentral(m_deviceInfo, this);

    auto st = m_state;
    QObject::connect(
        m_controller, &QLowEnergyController::connected, this, [this, st]() {
          if(!st->alive)
            return;
          m_is_open = true;
          if(!onConnected.isCallable())
            return;
          auto* engine = qjsEngine(this);
          if(!engine)
            return;
          onConnected.call({engine->newQObject(this)});
        });

    QObject::connect(
        m_controller, &QLowEnergyController::disconnected, this, [this, st]() {
          if(!st->alive)
            return;
          m_is_open = false;
          if(onDisconnected.isCallable())
            onDisconnected.call();
        });

    QObject::connect(
        m_controller, &QLowEnergyController::serviceDiscovered, this,
        [this, st](const QBluetoothUuid& uuid) {
          if(!st->alive)
            return;
          if(onServiceDiscovered.isCallable())
            onServiceDiscovered.call({uuid.toString(QUuid::WithoutBraces)});
        });

    QObject::connect(
        m_controller, &QLowEnergyController::discoveryFinished, this,
        [this, st]() {
          if(!st->alive)
            return;
          if(!onDiscoveryFinished.isCallable())
            return;
          auto* engine = qjsEngine(this);
          if(!engine)
            return;
          onDiscoveryFinished.call({engine->newQObject(this)});
        });

    QObject::connect(
        m_controller, &QLowEnergyController::mtuChanged, this,
        [this, st](int mtu) {
          if(!st->alive)
            return;
          if(onMtuChanged.isCallable())
            onMtuChanged.call({mtu});
        });

    QObject::connect(
        m_controller, &QLowEnergyController::errorOccurred, this,
        [this, st](QLowEnergyController::Error) {
          if(!st->alive)
            return;
          if(onError.isCallable())
            onError.call({m_controller->errorString()});
        });
  }

  ~qml_ble_controller()
  {
    m_state->alive = false;
    close();
  }

  void open() { m_controller->connectToDevice(); }

  void close()
  {
    if(m_is_open)
    {
      m_is_open = false;
      m_controller->disconnectFromDevice();
    }
  }
  W_SLOT(close)

  void discoverServices() { m_controller->discoverServices(); }
  W_SLOT(discoverServices)

  QObject* service(QString uuid)
  {
    auto serviceUuid = bleUuidFromString(uuid);
    auto* svc = m_controller->createServiceObject(serviceUuid, this);
    if(!svc)
      return nullptr;

    auto* wrapper = new qml_ble_service(svc, this);
    auto* engine = qjsEngine(this);
    if(engine)
      engine->newQObject(wrapper);
    return wrapper;
  }
  W_SLOT(service)

  QJSValue services()
  {
    auto* engine = qjsEngine(this);
    if(!engine)
      return {};

    auto uuids = m_controller->services();
    auto arr = engine->newArray(uuids.size());
    for(qsizetype i = 0; i < uuids.size(); ++i)
      arr.setProperty(i, uuids[i].toString(QUuid::WithoutBraces));
    return arr;
  }
  W_SLOT(services)

  int mtu() const { return m_controller->mtu(); }
  W_SLOT(mtu)

  QString remoteName() const { return m_controller->remoteName(); }
  W_SLOT(remoteName)

  QJSValue onConnected;
  QJSValue onDisconnected;
  QJSValue onServiceDiscovered;
  QJSValue onDiscoveryFinished;
  QJSValue onMtuChanged;
  QJSValue onError;

private:
  std::shared_ptr<state> m_state;
  QLowEnergyController* m_controller{};
  QBluetoothDeviceInfo m_deviceInfo;
  std::atomic_bool m_is_open{false};
};

} // namespace ossia::qt

#endif // __has_include(<QBluetoothDeviceDiscoveryAgent>)
