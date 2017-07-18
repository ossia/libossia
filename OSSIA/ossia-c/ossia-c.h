#ifndef OSSIA_H
#define OSSIA_H

#if defined(_MSC_VER)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#undef BOOL
#undef INT
#undef FLOAT
#undef CHAR
#endif

#include <ossia_export.h>
#if defined(__cplusplus)
extern "C"
{
#endif

struct ossia_protocol;
struct ossia_device;
struct ossia_domain;
struct ossia_value;

typedef ossia_protocol* ossia_protocol_t;
typedef ossia_device* ossia_device_t;
typedef ossia_domain* ossia_domain_t;
typedef ossia_value* ossia_value_t;
typedef void* ossia_node_t;
typedef void* ossia_address_t;


typedef void (*ossia_value_callback_t)(ossia_value_t);
struct ossia_value_callback_index;
typedef ossia_value_callback_index* ossia_value_callback_index_t;

enum ossia_type
{
  FLOAT_T,
  INT_T,
  VEC2F_T,
  VEC3F_T,
  VEC4F_T,
  IMPULSE_T,
  BOOL_T,
  STRING_T,
  TUPLE_T,
  CHAR_T
};

enum ossia_access_mode
{
  BI,
  GET,
  SET
};

enum ossia_bounding_mode
{
  FREE,
  CLIP,
  WRAP,
  FOLD,
  LOW,
  HIGH
};
//// Protocol ////


/**
 * @brief ossia_protocol_local_create
 * @return An instance of a Local protocol
 *
 * Protocols must be used during device creation.
 */
OSSIA_EXPORT
ossia_protocol_t ossia_protocol_local_create();

OSSIA_EXPORT
void ossia_protocol_local_expose_to(
    ossia_protocol_t local,
    ossia_protocol_t other);

OSSIA_EXPORT
ossia_protocol_t ossia_protocol_osc_create(
        const char* ip,
        int in_port,
        int out_port);

OSSIA_EXPORT
ossia_protocol_t ossia_protocol_minuit_create(
        const char* local_name,
        const char* ip,
        int in_port,
        int out_port);

OSSIA_EXPORT
void ossia_protocol_free(
    ossia_protocol_t);

//// Device ////
/**
 * @brief ossia_device_create
 *
 * ossia_device_create takes ownership
 * of the protocol ; it must not be used afterwards.
 */
OSSIA_EXPORT
ossia_device_t ossia_device_create(
        ossia_protocol_t protocol,
        const char* name);

OSSIA_EXPORT
void ossia_device_free(
        ossia_device_t device);

OSSIA_EXPORT
void ossia_device_reset_static();

OSSIA_EXPORT
bool ossia_device_update_namespace(
        ossia_device_t device);

OSSIA_EXPORT
ossia_node_t ossia_device_get_root_node(
        ossia_device_t device);

OSSIA_EXPORT
const char* ossia_device_get_name(
        ossia_device_t node);

//// Node ////
OSSIA_EXPORT
ossia_node_t ossia_node_add_child(
        ossia_node_t node,
        const char * name);

OSSIA_EXPORT
void ossia_node_remove_child(
        ossia_node_t node,
        ossia_node_t name);

OSSIA_EXPORT
const char* ossia_node_get_name(
        ossia_node_t node);

OSSIA_EXPORT
int ossia_node_child_size(
        ossia_node_t node);
OSSIA_EXPORT
ossia_node_t ossia_node_get_child(
        ossia_node_t node,
        int child_n);

OSSIA_EXPORT
ossia_address_t ossia_node_create_address(
        ossia_node_t node,
        ossia_type type);
OSSIA_EXPORT
void ossia_node_remove_address(
        ossia_node_t node);


//// Address ////
OSSIA_EXPORT
void ossia_address_set_access_mode(
        ossia_address_t address,
        ossia_access_mode am);
OSSIA_EXPORT
ossia_access_mode ossia_address_get_access_mode(
        ossia_address_t address);

OSSIA_EXPORT
void ossia_address_set_bounding_mode(
        ossia_address_t address,
        ossia_bounding_mode bm);
OSSIA_EXPORT
ossia_bounding_mode ossia_address_get_bounding_mode(
        ossia_address_t address);


OSSIA_EXPORT
void ossia_address_set_domain(
        ossia_address_t address,
        ossia_domain_t domain);
OSSIA_EXPORT
ossia_domain_t ossia_address_get_domain(
        ossia_address_t address);

OSSIA_EXPORT
void ossia_address_set_value(
        ossia_address_t address,
        ossia_value_t value);
OSSIA_EXPORT
ossia_value_t ossia_address_clone_value(
        ossia_address_t address);


OSSIA_EXPORT
void ossia_address_push_value(
        ossia_address_t address,
        ossia_value_t value);
OSSIA_EXPORT
ossia_value_t ossia_address_pull_value(
        ossia_address_t address);

OSSIA_EXPORT
ossia_value_callback_index_t ossia_address_add_callback(
        ossia_address_t address,
        ossia_value_callback_t callback);
OSSIA_EXPORT
void ossia_address_remove_callback(
        ossia_address_t address,
        ossia_value_callback_index_t index);



//// Domain ////
OSSIA_EXPORT
ossia_value_t ossia_domain_get_min(
        ossia_domain_t domain);
OSSIA_EXPORT
void ossia_domain_set_min(
        ossia_domain_t domain,
        ossia_value_t value);

OSSIA_EXPORT
ossia_value_t ossia_domain_get_max(
        ossia_domain_t domain);
OSSIA_EXPORT
void ossia_domain_set_max(
        ossia_domain_t domain,
        ossia_value_t value);

OSSIA_EXPORT
void ossia_domain_free(
        ossia_domain_t address);

//// Value ////
OSSIA_EXPORT
ossia_value_t ossia_value_create_impulse();
OSSIA_EXPORT
ossia_value_t ossia_value_create_int(int value);
OSSIA_EXPORT
ossia_value_t ossia_value_create_float(float value);
OSSIA_EXPORT
ossia_value_t ossia_value_create_bool(bool value);
OSSIA_EXPORT
ossia_value_t ossia_value_create_char(char value);
OSSIA_EXPORT
ossia_value_t ossia_value_create_string(const char* value);
OSSIA_EXPORT
ossia_value_t ossia_value_create_tuple(ossia_value_t* values, int size);

OSSIA_EXPORT
void ossia_value_free(ossia_value_t value);

OSSIA_EXPORT
ossia_type ossia_value_get_type(ossia_value_t type);
OSSIA_EXPORT
int ossia_value_to_int(ossia_value_t val);
OSSIA_EXPORT
float ossia_value_to_float(ossia_value_t val);
OSSIA_EXPORT
bool ossia_value_to_bool(ossia_value_t val);
OSSIA_EXPORT
char ossia_value_to_char(ossia_value_t val);
OSSIA_EXPORT
const char* ossia_value_to_string(ossia_value_t val);
OSSIA_EXPORT
void ossia_value_free_string(const char * str);
OSSIA_EXPORT
void ossia_value_to_tuple(ossia_value_t val_in, ossia_value_t* out, int* size);

OSSIA_EXPORT
void ossia_string_free(char*);


#if defined(__cplusplus)
}
#endif
#endif
