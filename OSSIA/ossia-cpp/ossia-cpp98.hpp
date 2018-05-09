#ifndef OSSIA_CPP98_HPP_2017_07_07
#define OSSIA_CPP98_HPP_2017_07_07
#include <stddef.h>
/** @defgroup CPP98API C++98 API
 * CPP98 bindings of libossia.
 *
 * @{
 */
/**
 * @file ossia-cpp98.hpp
 */

#include <ossia_export.h>
#include <string>
#include <vector>
#if (__cplusplus >= 201103L) || (defined(_MSC_VER) && (_MSC_VER >= 1900))
#define OSSIA_CPP_CXX11
#endif

namespace ossia
{
class value;
namespace net
{
class node_base;
class parameter_base;
class device_base;
}
}
/**
 *
 */
namespace opp
{
/**Access modes define if the parameter's value be accessed only on read, on
 * write, or bidirectionnally
 * @brief Defines R, W or RW access to the parameter's value
 */
enum access_mode
{
  Get, /// only read the value
  Set, /// only writethe value
  Bi   /// read  and write the value
};
/**Bounding modes define what to do at the bounds of the parameter's value
 * @brief Behaviour at the bounds of the value
 */
enum bounding_mode
{
  Free, /// Let the value go all the way both sides over the bounds
  Clip, /// Clip (clamp) the values at the bounds
  Wrap, /// Start over at min bound when going over max bound (and vice-versa)
  Fold, /// Fold back value when passing over the bound
  Low,  /// Clip the values only on the min bound
  High  /// Clip the values only at the max bound
};

class node;
class oscquery_server;
class oscquery_mirror;

//**************************************//
//               value                  //
//*************************************//

/**
 * @ingroup CPP98API
 * @brief The value class holds all ossia parameters typed values
 * @see ossia::value
 *  @{
 */

/**
 * @brief The value class holds all ossia parameters typed values
 * @see ossia::value
 */
class OSSIA_EXPORT value
{
  public:

    /** A container for a vector of 2 floats, supports sub-scripting
   * @brief container for a vector of 2 floats
   */
    struct vec2f {float data[2];
                  float& operator[](int i){return data[i];}};
    /** A container for a vector of 3 floats, supports sub-scripting
   * @brief container for a vector of 3 floats
     */
    struct vec3f {float data[3];
                  float& operator[](int i){return data[i];}};
    /** A container for a vector of 4 floats, supports sub-scripting
   * @brief container for a vector of 4 floats
     */
    struct vec4f {float data[4];
                  float& operator[](int i){return data[i];}};

    //*********************//
    //     Constructors    //
    //*********************//

    /**
     * @brief Value default constructors - creates an empty value
     */
    value();
    /**
     * @brief Value destructor
     */
    ~value();
    /**
     * @brief Copy constructor of a value
     * @param reference to another value
     */
    value(const value& v);
    /**
     * @brief Create a value of type int
     * @param an int
     * @see ossia::value
     */
    value(int v);
    /**
     * @brief Create a value of type bool
     * @see ossia::value
     */
    value(bool v);
    /**
     * @brief Create a value of type float
     * @see ossia::value
     */
    value(float v);
    /**
     * @brief Create a value of type double
     * @see ossia::value
     */
    value(double v);
    /**
     * @brief Create a value of type vec2f
     * @see ossia::value
     */
    value(const vec2f);
    /**
     * @brief Create a value of type vec3f
     * @see ossia::value
     */
    value(const vec3f);
    /**
     * @brief Create a value of type vec4f
     * @see ossia::value
     */
    value(const vec4f);
    /**
     * @brief Create a value of type list (vector of values)
     * @see ossia::value
     */
    value(std::vector<opp::value> v);
    /**
     * @brief Create a value of type std::string
     * @see ossia::value
     */
    value(std::string v);
    /**
     * @brief Create a value of type string (char*)
     * @see ossia::value
     */
    value(const char* v);


   //*********************//
   //     Type tests      //
   //*********************//


    /**
     * @brief Is the value of type impulse ?
     * @return True if the value is an impulse, false, otherwise
     */
    bool is_impulse() const;
    /**
     * @brief Is the value of type int ?
     * @return True if the value is an int, false, otherwise
     */
    bool is_int() const;
    /**
     * @brief Is the value of type float?
     * @return True if the value is a float, false, otherwise
     */
    bool is_float() const;
    /**
     * @brief Is the value of type boolean ?
     * @return True if the value is a boolean, false, otherwise
     */
    bool is_bool() const;
    /**
     * @brief Is the value of type vec2f ?
     * @return True if the value is a vec2f, false, otherwise
     */
    bool is_vec2f() const;
    /**
     * @brief Is the value of type vec3f ?
     * @return True if the value is a vec3f, false, otherwise
     */
    bool is_vec3f() const;
    /**
     * @brief Is the value of type vec4f ?
     * @return True if the value is a vec4f, false, otherwise
     */
    bool is_vec4f() const;
    /**
     * @brief Is the value of type list?
     * @return True if the value is a list, false, otherwise
     */
    bool is_list() const;
    /**
     * @brief Is the value of type string?
     * @return True if the value is a string, false, otherwise
     */
    bool is_string() const;


    //*********************//
    //     Convertors      //
    //*********************//

    /**
     * @brief converts the value to an int
     * @return value converted to int
     */
    int to_int() const;
    /**
     * @brief converts the value to a float
     * @return value converted to float
     */
    float to_float() const;
    /**
     * @brief converts the value to a boolean
     * @return value converted to boolean
     */
    bool to_bool() const;
    /**
     * @brief converts the value to a vec2f
     * @return value converted to vec2f
     */
    vec2f to_vec2f() const;
    /**
     * @brief converts the value to a vec3f
     * @return value converted to vec3f
     */
    vec3f to_vec3f() const;
    /**
     * @brief converts the value to a vec4f
     * @return value converted to vec4f
     */
    vec4f to_vec4f() const;
    /**
     * @brief converts the value to a list (vector of opp::value)
     * @return value converted to list (vector of opp::value)
     */
    std::vector<opp::value> to_list() const;
    /**
     * @brief converts the value to a string
     * @return value converted to std::string
     */
    std::string to_string() const;

    //*********************//
    //Assignment Operators //
    //*********************//

    /**
     * @brief operator = assigns with another opp::value
     * @param v : an opp::value
     * @return the reference to this value object
     */
    value& operator=(const value& v);
    /**
     * @brief operator = assigns with an integer
     * @param v : an int
     * @return the reference to this value object
     */
    value& operator=(int v);
    /**
     * @brief operator = assigns with a float
     * @param v : a float
     * @return the reference to this value object
     */
    value& operator=(float v);
    /**
     * @brief operator = assigns with a bool
     * @param v : a bool
     * @return the reference to this value object
     */
    value& operator=(bool v);
    /**
     * @brief operator = assigns with a vec2f
     * @param v : a vec2f
     * @return the reference to this value object
     */
    value& operator=(vec2f v);
    /**
     * @brief operator = assigns with a vec3f
     * @param v : a vec3f
     * @return the reference to this value object
     */
    value& operator=(vec3f v);
    /**
     * @brief operator = assigns with a vec4f
     * @param v : a vec4f
     * @return the reference to this value object
     */
    value& operator=(vec4f v);
    /**
     * @brief operator = assigns with a vector of values (aka list)
     * @param v : a vector of values (aka list)
     * @return the reference to this value object
     */
    value& operator=(std::vector<opp::value> v);
    /**
     * @brief operator = assigns with a string
     * @param v : a string
     * @return the reference to this value object
     */
    value& operator=(std::string v);

    /**
     * @brief sets the value as an impulse (a value-less signal)
     */
    void set_impulse();
    /**
     * @brief sets the value as an int
     * @param v int is moved intoto the value object
     */
    void set_int(int v);
    /**
     * @brief sets the value as a float
     * @param v float is moved intoto the value object
     */
    void set_float(float v);
    /**
     * @brief sets the value as a bool
     * @param v bool is moved intoto the value object
     */
    void set_bool(bool v);
    /**
     * @brief sets the value as a vec2f
     * @param v vec2f is moved intoto the value object
     */
    void set_vec2f(vec2f v);
    /**
     * @brief sets the value as a vec3f
     * @param v vec3f is moved intoto the value object
     */
    void set_vec3f(vec3f v);
    /**
     * @brief sets the value as a vec4f
     * @param v vec4f is moved intoto the value object
     */
    void set_vec4f(vec4f v);
    /**
     * @brief sets the value as a vector of opp::values
     * @param v vector of opp::values is moved intoto the value object
     */
    void set_list(std::vector<opp::value> v);
    /**
     * @brief sets the value as a string
     * @param v string is moved intoto the value object
     */
    void set_string(std::string v);

#if defined(OSSIA_CPP_CXX11)
    value(value&& v);
    value& operator=(value&& v);
#endif
  private:
    friend class node;
    value(const ossia::value& v);
    template<typename T>
    value(const T* v);
    ossia::value* m_val;
};

/** @}*/

typedef void (*value_callback)(void*, const opp::value&);
typedef void (*connection_callback)(void*, const std::string&);
typedef void (*disconnection_callback)(void*, const std::string&);

/**
 * @ingroup CPP98API
 * @brief he callback_index struct holds callbacks for adding listeners to parameters' values
 *
 *  @{
 */

/**
 * @brief The callback_index struct holds callbacks for adding listeners to parameters' values
 */
struct OSSIA_EXPORT callback_index {
    callback_index();
    ~callback_index();
    callback_index(const callback_index&);
    callback_index& operator=(const callback_index&);
    operator bool() const;

  private:
    friend class node;

    struct impl;
    impl* index;
};

/** @}*/

/**
 * @ingroup CPP98API
 * @brief The node class holds all ossia parameters typed values
 * @see ossia::value
 *  @{
 */

/**
* @brief The node class holds all ossia  nodes and their optionnal parameters
*/
class OSSIA_EXPORT node
{
  public:

    //*********************//
    //     Constructors    //
    //*********************//

    /**
     * @brief node default constructor
     */
    node();
    /**
     * @brief node copy constructor
     */
    node(const node&);
    /**
     * @brief operator = assign the current node to another one
     * @return reference to the assigned node
     */
    node& operator=(const node&);

    /**
     * @brief node default destructor
     */
    ~node();

    /**
     * @brief operator bool checks is the node is valid
     * @return true if the node is valid
     */
    operator bool() const;

    /**
     * @brief get_name allows to get a node's name
     * @return a string with the node's name
     */
    std::string get_name() const;
    /**
     * @brief set_name allows to set or change a node's name
     * @param a string with the desired node name
     */
    void set_name(std::string s);

    std::string get_parameter() const;
    bool has_parameter() const;

    /**
   * @brief get_namespace returns all direct children (one level of hierarchy only)
   * @return opp::node vector
   */
    std::vector<node> get_children() const;
    /**
   * @brief get_namespace return all children recursively by priority order
   * @return opp::node vector
   */
    std::vector<node> get_namespace() const;

    node find_child(std::string addr);
    void remove_child(std::string addr);
    void remove_children();

    // Create a node
    node create_child(std::string addr);

    // And optionnally add a typed parameter to it
    void set_impulse();
    void set_int();
    void set_float();
    void set_bool();
    void set_list();

    void set_vec2f();
    void set_vec3f();
    void set_vec4f();

    void set_string();

    void set_buffer();
    void set_filepath();

    void set_rgb();
    void set_rgba();
    void set_argb();
    void set_argb8();
    void set_hsv();

    void set_cart2D();
    void set_cart3D();
    void set_polar();
    void set_spherical();
    void set_opengl();
    void set_cylindrical();

    void set_quaternion();
    void set_euler();
    void set_axis();

    void set_decibel();
    void set_midigain();


    // Or directly create the child node and its parameter in one row:
    node create_impulse(std::string addr);
    node create_int(std::string addr);
    node create_float(std::string addr);
    node create_bool(std::string addr);
    node create_list(std::string addr);

    node create_vec2f(std::string addr);
    node create_vec3f(std::string addr);
    node create_vec4f(std::string addr);

    node create_string(std::string addr);

    node create_buffer(std::string addr);
    node create_filepath(std::string addr);

    node create_rgb(std::string addr);
    node create_rgba(std::string addr);
    node create_argb(std::string addr);
    node create_argb8(std::string addr);
    node create_hsv(std::string addr);

    node create_cart2D(std::string addr);
    node create_cart3D(std::string addr);
    node create_polar(std::string addr);
    node create_spherical(std::string addr);
    node create_opengl(std::string addr);
    node create_cylindrical(std::string addr);

    node create_quaternion(std::string addr);
    node create_euler(std::string addr);
    node create_axis(std::string addr);

    node create_decibel(std::string addr);
    node create_midigain(std::string addr);

    // Set value and attributes of nodes and parameters:
    node& set_value(opp::value v);
    opp::value get_value() const;
    opp::value fetch_value() const;

    callback_index set_value_callback(value_callback c, void* ctx);
    void remove_value_callback(callback_index idx);

    node& set_min(opp::value min);
    opp::value get_min() const;

    node& set_max(opp::value max);
    opp::value get_max() const;

    node& set_accepted_values(std::vector<opp::value> v);
    std::vector<opp::value> get_accepted_values() const;

    node& set_unit(std::string v);
    std::string get_unit() const;

    node& set_access(access_mode v);
    access_mode get_access() const;

    node& set_bounding(bounding_mode v);
    bounding_mode get_bounding() const;

    node& set_default_value(value v);
    value get_default_value();

    node& set_description(std::string v);
    std::string get_description() const;

    node& set_tags(std::vector<std::string> v);
    std::vector<std::string> get_tags() const;

    node& set_instance_bounds(int min, int max);
    node& unset_instance_bounds();
    std::pair<int, int> get_instance_bounds() const;

    node& set_priority(float v);
    node& unset_priority();
    float get_priority();

    node& set_refresh_rate(int v);
    node& unset_refresh_rate();
    int get_refresh_rate();

    node& set_value_step_size(double v);
    node& unset_value_step_size();
    double get_value_step_size();

    node& set_hidden(bool v);
    bool get_hidden() const;

    node& set_disabled(bool v);
    bool get_disabled() const;

    node& set_critical(bool v);
    bool get_critical() const;

    bool get_zombie() const;

    node& set_muted(bool v);
    bool get_muted() const;

    node& set_repetition_filter(bool v);
    bool get_repetition_filter() const;

#if defined(OSSIA_CPP_CXX11)
    node(node&&) = default;
    node& operator=(node&&) = default;
#endif

  private:
    friend class oscquery_mirror;
    friend class oscquery_server;
    node(ossia::net::node_base* b);
    node(ossia::net::node_base* b, ossia::net::parameter_base* addr);
    void init();
    void cleanup(const ossia::net::node_base&);
    void cleanup_parameter(const ossia::net::parameter_base&);

    ossia::net::node_base* m_node;
    ossia::net::parameter_base* m_param;
};

/** @}*/

class OSSIA_EXPORT oscquery_server
{
  public:
    oscquery_server();
    oscquery_server(std::string name, int oscPort = 1234, int wsPort = 5678);
    ~oscquery_server();

    void setup(std::string name, int oscPort = 1234, int wsPort = 5678);
    node get_root_node() const;

    void set_connection_callback(connection_callback c, void* ctx);
    void remove_connection_callback();

    void set_disconnection_callback(disconnection_callback c, void* ctx);
    void remove_disconnection_callback();

  private:
    void on_connection(const std::string&);
    void on_disconnection(const std::string&);
    ossia::net::device_base* m_dev;
    connection_callback m_con;
    void* m_con_ctx;
    disconnection_callback m_discon;
    void* m_discon_ctx;
};

class OSSIA_EXPORT oscquery_mirror
{
  public:
    oscquery_mirror(std::string name, std::string host = "ws://127.0.0.1:5678");
    ~oscquery_mirror();

    node get_root_node() const;
    void refresh();
    void reconnect(std::string name, std::string host);

  private:
    ossia::net::device_base* m_dev;
};
}

/** @}*/

#endif


/** @}*/
