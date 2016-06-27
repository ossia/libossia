#pragma once

namespace OSSIA
{
#if 0
# pragma mark -
# pragma mark Enumerations
#endif

  /*! operation allowed on address */
  enum class AccessMode
  {
    GET,
    SET,
    BI
  };

  /*! address behaviors at crossing domain boundaries time */
  enum class BoundingMode
  {
    FREE,
    CLIP,
    WRAP,
    FOLD
  };
}
