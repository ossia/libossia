#include "BasicOSC.h"

#include <BasicDevice.h>

namespace impl
{

shared_ptr<OSC> OSC::create(std::string ip, int in_port, int out_port)
{
  return make_shared<OSC2>(ip, in_port, out_port);
}

OSC2::OSC2(std::string ip, int in_port, int out_port) :
mIp(ip),
mInPort(in_port),
mOutPort(out_port)
{
}

OSC2::~OSC2()
{}


std::string OSC2::getIp() const
{
  return mIp;
}

Protocol & OSC2::setIp(std::string ip)
{
  mIp = ip;

  return *this;
}

int OSC2::getInPort() const
{
  return mInPort;
}

Protocol & OSC2::setInPort(int in_port)
{
  mInPort = in_port;
  return *this;
}

int OSC2::getOutPort() const
{
  return mOutPort;
}

Protocol & OSC2::setOutPort(int out_port)
{
  mOutPort = out_port;
  return *this;
}

bool OSC2::getLearningStatus() const
{
    return mLearning;
}


Protocol& OSC2::setLearningStatus(OSSIA::Device& ossiaDevice, bool newLearn)
{
    return *this;
}

bool OSC2::updateChildren(Node& node) const
{
  return false;
}
}
