#ifndef OSSIA_H
#define OSSIA_H

#if defined(__cplusplus)
extern "C"
{
#endif

struct ossia_protocol;
struct ossia_device;
struct ossia_node;
struct ossia_address;
struct ossia_domain;

typedef ossia_protocol* ossia_protocol_t;
typedef ossia_device* ossia_device_t;
typedef ossia_node* ossia_node_t;
typedef ossia_address* ossia_address_t;
typedef void* ossia_value_t;
typedef ossia_domain* ossia_domain_t;


typedef void (*ossia_value_callback_t)(ossia_value_t);
struct ossia_value_callback_index;
typedef ossia_value_callback_index* ossia_value_callback_index_t;

enum ossia_type
{
  IMPULSE,
  BOOL,
  INT,
  FLOAT,
  CHAR,
  STRING,
  TUPLE,
  GENERIC,
  DESTINATION,
  BEHAVIOR
};

enum ossia_access_mode
{
  GET,
  SET,
  BI
};

enum ossia_bounding_mode
{
  FREE,
  CLIP,
  WRAP,
  FOLD
};
//// Protocol ////


/**
 * @brief ossia_protocol_local_create
 * @return An instance of a Local protocol
 *
 * Protocols must be used during device creation.
 */
ossia_protocol_t ossia_protocol_local_create();

ossia_protocol_t ossia_protocol_local_expose_to(
    ossia_protocol_t other);

ossia_protocol_t ossia_protocol_osc_create(
        const char* ip,
        int in_port,
        int out_port);
ossia_protocol_t ossia_protocol_minuit_create(
        const char* ip,
        int in_port,
        int out_port);

//// Device ////
/**
 * @brief ossia_device_create
 *
 * ossia_device_create takes ownership
 * of the protocol ; it must not be used afterwards.
 */
ossia_device_t ossia_device_create(
        ossia_protocol_t protocol,
        const char* name);

void ossia_device_free(
        ossia_device_t device);

bool ossia_device_update_namespace(
        ossia_device_t device);

ossia_node_t ossia_device_get_root_node(
        ossia_device_t device);

const char* ossia_device_get_name(
        ossia_device_t node);

//// Node ////
ossia_node_t ossia_node_add_child(
        ossia_node_t node,
        const char * name);
void ossia_node_remove_child(
        ossia_node_t node,
        ossia_node_t name);

void ossia_node_free(
        ossia_node_t node);

const char* ossia_node_get_name(
        ossia_node_t node);

int ossia_node_child_size(
        ossia_node_t node);
ossia_node_t ossia_node_get_child(
        ossia_node_t node,
        int child_n);

ossia_address_t ossia_node_create_address(
        ossia_node_t node,
        ossia_type type);
void ossia_node_remove_address(
        ossia_node_t node,
        ossia_address_t address);


//// Address ////
void ossia_address_set_access_mode(
        ossia_address_t address,
        ossia_access_mode am);
ossia_access_mode ossia_address_get_access_mode(
        ossia_address_t address);

void ossia_address_set_bounding_mode(
        ossia_address_t address,
        ossia_bounding_mode bm);
ossia_bounding_mode ossia_address_get_bounding_mode(
        ossia_address_t address);

void ossia_address_set_domain(
        ossia_address_t address,
        ossia_domain_t domain);
ossia_domain_t ossia_address_get_domain(
        ossia_address_t address);

void ossia_address_set_value(
        ossia_address_t address,
        ossia_value_t value);
ossia_value_t ossia_address_get_value(
        ossia_address_t address);


void ossia_address_push_value(
        ossia_address_t address,
        ossia_value_t value);
ossia_value_t ossia_address_pull_value(
        ossia_address_t address);

ossia_value_callback_index_t ossia_address_add_callback(
        ossia_address_t address,
        ossia_value_callback_t callback);
void ossia_address_remove_callback(
        ossia_address_t address,
        ossia_value_callback_index_t index);



//// Domain ////
ossia_value_t ossia_domain_get_min(
        ossia_domain_t domain);
void ossia_domain_set_min(
        ossia_domain_t domain,
        ossia_value_t value);

ossia_value_t ossia_domain_get_max(
        ossia_domain_t domain);
void ossia_domain_set_max(
        ossia_domain_t domain,
        ossia_value_t value);

void ossia_domain_free(
        ossia_domain_t address);

//// Value ////
ossia_value_t ossia_value_create_impulse();
ossia_value_t ossia_value_create_int(int value);
ossia_value_t ossia_value_create_float(float value);
ossia_value_t ossia_value_create_bool(bool value);
ossia_value_t ossia_value_create_char(char value);
ossia_value_t ossia_value_create_string(const char* value);
ossia_value_t ossia_value_create_tuple(ossia_value_t* values, int size);

void ossia_value_free(ossia_value_t value);

ossia_type ossia_value_get_type(ossia_value_t type);
int ossia_value_to_int(ossia_value_t val);
float ossia_value_to_float(ossia_value_t val);
bool ossia_value_to_bool(ossia_value_t val);
char ossia_value_to_char(ossia_value_t val);
const char* ossia_value_to_string(ossia_value_t val);
void ossia_value_free_string(const char * str);
void ossia_value_to_tuple(ossia_value_t val_in, ossia_value_t* out, int* size);

void ossia_string_free(char*);


#if defined(__cplusplus)
}
#endif
#endif
