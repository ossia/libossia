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
  Minuit(std::string name, std::string ip, int port)
    :name(name), ip(ip), port(port) {}

  std::string name;
  std::string ip;
  int port;
};


class OSC : public Protocol {
  OSC(std::string ip, int in_port, int out_port)
    :ip(ip), in_port(in_port), out_port(out_port) {}

  std::string ip;
  int in_port;
  int out_port;
};

}

#endif /* PROTOCOLS_H_ */
