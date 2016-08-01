#pragma once
#include <memory>
#include <ossia_export.h>

namespace ossia
{
class network_logger;
class state;

namespace net
{
class address_base;
class node_base;
class device_base;

/**
 * @brief The protocol_base class
 *
 * A protocol is used to implement remote communication.
 *
 * \see local_protocol
 * \see osc_protocol
 * \see minuit_protocol
 * \see midi_protocol
 */
class OSSIA_EXPORT protocol_base
{
public:
  virtual ~protocol_base();

  virtual bool pull(address_base&) = 0;
  virtual bool push(const address_base&) = 0;
  virtual bool pushState(const state&)
  {
    return false;
  }

  virtual bool observe(address_base&, bool) = 0;
  virtual bool observe_quietly(address_base&, bool)
  { return false; }

  virtual bool update(node_base& node_base) = 0;

  virtual void setDevice(ossia::net::device_base& dev)
  {
  }

  void setLogger(const std::shared_ptr<network_logger>& l)
  {
    mLogger = l;
  }
  const std::shared_ptr<network_logger>& getLogger() const
  {
    return mLogger;
  }

protected:
  std::shared_ptr<ossia::network_logger> mLogger;
};
}
}
