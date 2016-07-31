#pragma once

namespace ossia
{
/*! type of value */
enum class val_type
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
enum class access_mode
{
    GET,
    SET,
    BI
};

/*! address behaviors at crossing domain boundaries time */
enum class bounding_mode
{
    FREE,
    CLIP,
    WRAP,
    FOLD
};

enum class repetition_filter
{
    OFF, ON
};
}
