#include "Network/Protocol/JamomaMinuit.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Life Cycle

shared_ptr<Minuit> Minuit::create(std::string ip, int in_port, int out_port)
{
  return make_shared<JamomaMinuit>(ip, in_port, out_port);
}

JamomaMinuit::JamomaMinuit(std::string ip, int in_port, int out_port) :
mIp(ip),
mInPort(in_port),
mOutPort(out_port)
{}

JamomaMinuit::~JamomaMinuit()
{}

# pragma mark -
# pragma mark Accessors

std::string JamomaMinuit::getIp()
{
  return mIp;
}

int JamomaMinuit::getInPort()
{
  return mInPort;
}

int JamomaMinuit::getOutPort()
{
  return mOutPort;
}

# pragma mark -
# pragma mark Operation

bool JamomaMinuit::pullAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool JamomaMinuit::pushAddressValue(std::shared_ptr<Address>) const
{
  return false;
}

bool JamomaMinuit::observeAddressValue(std::shared_ptr<Address>, bool) const
{
  return false;
}
