#pragma once
#include <utility>
#include <ossia_export.h>

namespace OSSIA
{

enum class CurveSegmentType { INT, FLOAT, DOUBLE, BOOL };
using CurveType = std::pair<OSSIA::CurveSegmentType, OSSIA::CurveSegmentType>;

class OSSIA_EXPORT CurveAbstract
{
public:
  /*! destructor
   \todo remove = default */
  virtual ~CurveAbstract() = default;

  /*! get the type of both coordinates */
  virtual CurveType getType() const = 0;

  /*! Clear internal data structures of the curve.
   * Call before a new execution. */
  virtual void reset() = 0;
};

}
