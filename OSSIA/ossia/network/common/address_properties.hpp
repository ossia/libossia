#pragma once
#include <ossia/detail/config.hpp>
#include <cstdint>
namespace ossia
{
/**
 * @brief Enum to represent the types that a value can take.
 *
 * @note If changing this, also take care of changing :
 * * val_type in qml_context
 * * ossia_type in ossia-c.h
 * * ossia::val_type */
enum class val_type : int8_t
{
  FLOAT, //! \see Float
  INT, //! \see Int
  VEC2F, //! \see Vec2f
  VEC3F, //! \see Vec3f
  VEC4F, //! \see Vec4f
  IMPULSE, //! \see Impulse
  BOOL, //! \see Bool
  STRING, //! \see String
  TUPLE, //! \see Tuple
  CHAR,
  NONE
};


/**
 * @brief Address behaviors at crossing domain boundaries time
 */
enum class access_mode : int8_t
{
  GET, //! The value can be retrieved
  SET, //! The value can be changed
  BI //! The value can be retrieved and changed
};

/**
 * @brief Address behaviors at crossing domain boundaries.
 */
enum class bounding_mode : int8_t
{
  FREE, //! The bounds are ignored
  CLIP, //! \see ossia::clamp
  WRAP, //! \see ossia::wrap
  FOLD, //! \see ossia::fold
  LOW, //! \see ossia::clamp_min
  HIGH, //! \see ossia::clamp_max
};

/**
 * @brief If enabled, sending twice the same value will only send it once by network
 */
enum repetition_filter : bool
{
  OFF = false,
  ON = true
};
}
