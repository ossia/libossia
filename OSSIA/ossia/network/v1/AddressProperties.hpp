#pragma once

namespace OSSIA
{
#if 0
# pragma mark -
# pragma mark Enumerations
#endif


/*! type of value */
enum class Type
{
    IMPULSE,
    BOOL,
    INT,
    FLOAT,
    CHAR,
    STRING,
    TUPLE,
    VEC2F,
    VEC3F,
    VEC4F,
    DESTINATION,  //! \note see Destination structure declaration in Node.h
    BEHAVIOR,     //! \note see Behavior structure declaration in Curve.h
};

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
