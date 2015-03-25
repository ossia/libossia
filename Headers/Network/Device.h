/*!
 * \file Device.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include <string>

#include "Network/Node.h"

namespace OSSIA {

class Protocol;

class Device : public virtual Node {

public:

  // Factory, destructor
  static std::shared_ptr<Device> create(Protocol &, std::string = "");
  virtual ~Device() = default;

  // Network
  virtual bool updateNamespace() = 0;
  
};

std::shared_ptr<Node> declareMinuitInternDevice(std::string, int port);
std::shared_ptr<Node> declareOSCInternDevice(int in_port, int out_port);

}

#endif /* DEVICE_H_ */
