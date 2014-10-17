/*!
 * \file Protocol.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef PROTOCOLS_H_
#define PROTOCOLS_H_

namespace OSSIA {

class Minuit {

public:

  // Constructors, destructor, assignment
  Minuit(std::string name, std::string ip, int port);
  Minuit(const Minuit&) = delete;
  ~Minuit();
  Minuit & operator= (const Minuit&) = delete;

};



class OSC {

public:

  // Constructors, destructor, assignment
  OSC(std::string ip, int in_port, int out_port);
  OSC(const OSC&) = delete;
  ~OSC();
  OSC & operator= (const OSC&) = delete;

};

}

#endif /* PROTOCOLS_H_ */
