#pragma once
#include <vector>
#include <string>
namespace ossia
{
/**
 * @brief Enum to represent the types that a value can take.
 */
enum class val_type
{
  IMPULSE, //! \see Impulse
  BOOL, //! \see Bool
  INT, //! \see Int
  FLOAT, //! \see Float
  CHAR, //! \see Char
  STRING, //! \see String
  TUPLE, //! \see Tuple
  VEC2F, //! \see Vec2f
  VEC3F, //! \see Vec3f
  VEC4F, //! \see Vec4f
  DESTINATION, //! \see Destination
  BEHAVIOR,    //! \see Behavior
};


/**
 * @brief Address behaviors at crossing domain boundaries time
 */
enum class access_mode
{
  GET, //! The value can be retrieved
  SET, //! The value can be changed
  BI //! The value can be retrieved and changed
};

/**
 * @brief Address behaviors at crossing domain boundaries.
 */
enum class bounding_mode
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
enum class repetition_filter
{
  OFF,
  ON
};

using tags_t = std::vector<std::string>;
struct dataspace_t
{

};

struct dataspace_unit_t
{

};
}
