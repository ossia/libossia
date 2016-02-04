#pragma once

namespace OSSIA
{
# pragma mark -
# pragma mark Enumerations

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
