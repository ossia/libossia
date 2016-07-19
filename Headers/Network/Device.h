/*!
 * \file Device.h
 *
 * \defgroup Network
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <string>

#include "Network/Node.h"
#include <ossia_export.h>

namespace OSSIA
{

class Protocol;

class OSSIA_EXPORT Device : public virtual Node
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \param std::shared_ptr<Protocol>
   \param device name
   \return std::shared_ptr<#Device> */
  static std::shared_ptr<Device> create(std::shared_ptr<Protocol>, std::string = "");

  /*! destructor */
  virtual ~Device();

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get device's protocol
   \return std::shared_ptr<#Protocol> */
  virtual std::shared_ptr<Protocol> getProtocol() const = 0;

  /*! build the namespace from the root
   \deprecated use Protocol::updateChildren
   \return bool true if the namespace update succeeded */
  virtual bool updateNamespace() = 0;
};

/*! declare Minuit internal device
\note is this still useful ?
\param int in_port where to receive messages
\param int out_port where to send messages
\return std::shared_ptr<Node ??? */
OSSIA_EXPORT std::shared_ptr<Node> declareMinuitInternDevice(int in_port, int out_port);

/*! declare OSC internal device
\note is this still useful ?
\param int in_port where to receive messages
\param int out_port where to send messages
\return std::shared_ptr<Node ??? */
OSSIA_EXPORT std::shared_ptr<Node> declareOSCInternDevice(int in_port, int out_port);

}
