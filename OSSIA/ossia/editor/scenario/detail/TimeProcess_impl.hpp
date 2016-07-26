/*!
 * \file TimeProcess_impl.h
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

#include <ossia/editor/scenario/time_value.hpp>

namespace impl
{
class JamomaTimeProcess
{
protected:
  OSSIA::TimeValue mLastDate;  // used to filter multiple state calls at the same time (use date as position can be always 0 in infinite duration case)

public:
  JamomaTimeProcess() :
      mLastDate(OSSIA::Infinite)
  {
  }
};
}
