/*!
 * \file JamomaLocal.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include "Network/Protocol/Local.h"

#include "Network/JamomaProtocol.h"

#include "TTModular.h"

using namespace OSSIA;
using namespace std;

class JamomaLocal : public Local, public JamomaProtocol
{
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaLocal();
  
  ~JamomaLocal();
};
