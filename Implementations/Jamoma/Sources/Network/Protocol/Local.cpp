#include "Network/Protocol/JamomaLocal.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Life Cycle

shared_ptr<Local> Local::create()
{
  return make_shared<JamomaLocal>();
}

JamomaLocal::JamomaLocal()
{}

JamomaLocal::~JamomaLocal()
{}
