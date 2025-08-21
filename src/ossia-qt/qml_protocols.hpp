#pragma once

#include <ossia/detail/config.hpp>

#include <ossia/network/context_functions.hpp>

#include <QJSValue>
#include <QObject>
#include <QVariant>

#include <verdigris>

namespace ossia::qt
{

class OSSIA_EXPORT qml_protocols : public QObject
{
  W_OBJECT(qml_protocols)
public:
  explicit qml_protocols(ossia::net::network_context_ptr ctx, QObject* parent);
  ~qml_protocols() override;

  // All sockets have:
  // onOpen
  // onClose
  // onError

  // write
  QObject* outboundUDP(QVariant config);
  W_SLOT(outboundUDP)

  // onMessage
  QObject* inboundUDP(QVariant config);
  W_SLOT(inboundUDP)

  // write
  QObject* outboundUnixDatagram(QVariant config);
  W_SLOT(outboundUnixDatagram)

  // onMessage
  QObject* inboundUnixDatagram(QVariant config);
  W_SLOT(inboundUnixDatagram)

  // write
  QObject* outboundUnixStream(QVariant config);
  W_SLOT(outboundUnixStream)

  // onMessage
  QObject* inboundUnixStream(QVariant config);
  W_SLOT(inboundUnixStream)

  // write
  // onBytes
  QObject* outboundTCP(QVariant config);
  W_SLOT(outboundTCP)

  // onConnection
  // -> onBytes
  // -> write
  QObject* inboundTCP(QVariant config);
  W_SLOT(inboundTCP)

  // write
  // onBytes
  QObject* outboundWS(QVariant config);
  W_SLOT(outboundWS)

  // onConnection
  // -> onBytes
  // -> onTextMessage
  // -> onBinaryMessage
  QObject* inboundWS(QVariant config);
  W_SLOT(inboundWS)

  void http(QUrl url, QJSValue func, QString verb);
  W_SLOT(http);

  QJSValue inboundMIDIDevices();
  W_SLOT(inboundMIDIDevices)
  QJSValue inboundUMPDevices();
  W_SLOT(inboundUMPDevices)

  QObject* inboundMIDI(QJSValue config);
  W_SLOT(inboundMIDI)
  QObject* inboundUMP(QJSValue config);
  W_SLOT(inboundUMP)

  QJSValue outboundMIDIDevices();
  W_SLOT(outboundMIDIDevices)
  QJSValue outboundUMPDevices();
  W_SLOT(outboundUMPDevices)

  QObject* outboundMIDI(QJSValue config);
  W_SLOT(outboundMIDI)
  QObject* outboundUMP(QJSValue config);
  W_SLOT(outboundUMP)

  QObject* serial(QVariant config);
  W_SLOT(serial)

  QObject* osc(QVariant config);
  W_SLOT(osc)

private:
  ossia::net::network_context_ptr context;
};

}
