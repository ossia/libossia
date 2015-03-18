/*!
 * \file Protocols.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef PROTOCOLS_H_
#define PROTOCOLS_H_

#include <string>

namespace OSSIA {

class Protocol {};


struct Local : public Protocol {};


struct Minuit : public Protocol {
  std::string name;
  std::string ip;
  int port;
};


class OSC : public Protocol {
  std::string ip;
  int in_port;
  int out_port;
};

}

#endif /* PROTOCOLS_H_ */
