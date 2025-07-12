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

  QObject* inboundMIDI(QVariant config);
  W_SLOT(inboundMIDI)
  QObject* inboundUMP(QVariant config);
  W_SLOT(inboundUMP)

  QObject* outboundMIDI(QVariant config);
  W_SLOT(outboundMIDI)
  QObject* outboundUMP(QVariant config);
  W_SLOT(outboundUMP)

  QObject* serial(QVariant config);
  W_SLOT(serial)

private:
  ossia::net::network_context_ptr context;
};

}
