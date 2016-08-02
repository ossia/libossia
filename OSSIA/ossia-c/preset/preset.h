#ifndef _ossia_API_NAMESPACE_
#define _ossia_API_NAMESPACE_

#include "result.cs"

#if defined(__cplusplus)
extern "C"
{
#endif

struct ossia_preset;
struct ossia_device;
struct ossia_node;
struct ossia_address;
typedef ossia_device* ossia_device_t;
typedef ossia_preset* ossia_preset_t;
typedef ossia_node* ossia_node_t;
typedef ossia_address* ossia_address_t;
typedef enum ossia_preset_result_enum ossia_preset_result;

/// Preset handling ///
/**
 * @brief ossia_presets_read_json
 * @param A string containing json data of a preset.
 * @param A preset receiving the new preset instance
 * @return A result code
 */
ossia_preset_result ossia_presets_read_json (
        const char*,
        ossia_preset_t*);

/**
 * @brief ossia_presets_read_xml
 * @param A string containing xml data of a preset.
 * @param a preset receiving the new preset instance
 * @return A result code
 */
ossia_preset_result ossia_presets_read_xml(
        const char*,
        ossia_preset_t*);

/**
 * @brief ossia_preset_free
 * @param An allocated preset
 * @return A result code
 *
 * Frees a preset.
 * Has to be called for presets allocated with
 *  ossia_presets_read_xml
 *  ossia_presets_read_json
 */

ossia_preset_result ossia_presets_free(
        ossia_preset_t);


/**
 * @brief ossia_presets_write_json
 * @param a preset
 * @param a buffer receiving the text json version of the preset
 * @return A result code
 */

ossia_preset_result ossia_presets_write_json(
        const ossia_preset_t,
        const char**);
/**
 * @brief ossia_presets_write_xml
 * @param a preset
 * @param a buffer receiving the text xml version of the preset
 * @return A result code
 */

ossia_preset_result ossia_presets_write_xml(
        const ossia_preset_t,
        const char**);
/**
 * @brief ossia_presets_size
 * @param a preset
 * @param a buffer receiving the preset's size
 * @return A result code
 */

ossia_preset_result ossia_presets_size(
        const ossia_preset_t,
        int*);

/**
 * @brief ossia_presets_to_string
 * @param a preset
 * @param a buffer receiving a string representation of the preset
 * @return A result code
 */

ossia_preset_result ossia_presets_to_string(
        const ossia_preset_t,
        const char **);

/// Device handling ///

/**
 * @brief ossia_devices_read_json
 * @param an existing ossia device
 * @param textual json of device data
 * @return A result code
 *
 * The device will be cleared and replaced
 * with this data.
 */
ossia_preset_result ossia_devices_read_json(
        ossia_device_t*,
        const char*);

/**
 * @brief ossia_devices_read_xml
 * @param an existing ossia device
 * @param textual xml of device data
 * @return A result code
 *
 * The device will be cleared and replaced
 * with this data.
 */
ossia_preset_result ossia_devices_read_xml(
        ossia_device_t*,
        const char*);

/**
 * @brief ossia_devices_write_json
 * @param an existing device
 * @param a buffer receiving a json-formatted text string corresponding to this data
 * @return A result code
 */

ossia_preset_result ossia_devices_write_json(
        const ossia_device_t,
        const char**);

/**
 * @brief ossia_devices_write_json
 * @param an existing device
 * @param a buffer receiving an xml-formatted text string corresponding to this data
 * @return A result code
 */
ossia_preset_result ossia_devices_write_xml(
        const ossia_device_t,
        const char**);


/**
 * @brief ossia_devices_apply_preset
 * @param an existing device
 * @param an existing preset
 * @return A result code
 *
 * Applies the values of a preset into a device
 */
ossia_preset_result ossia_devices_apply_preset(
        ossia_device_t,
        ossia_preset_t,
        bool keep_arch = true);

/**
 * @brief ossia_devices_make_preset
 * @param an existing device
 * @param a preset receiving a preset corresponding to the current values in the device
 * @return A result code
 */
ossia_preset_result ossia_devices_make_preset(
        ossia_device_t,
        ossia_preset_t*);

/**
 * @brief ossia_devices_to_string
 * @param an existing device
 * @param a buffer receiving a string representation of the device
 * @return A result code
 */

ossia_preset_result ossia_devices_to_string(
        ossia_device_t,
        const char**);

/// Miscellaneous ///

/**
 * @brief ossia_device_get_node
 * @param an existing device
 * @param a string corresponding to the address of a node
 * @param a node receiving the node at this address if it exists
 * @return A result code
 */

ossia_preset_result ossia_devices_get_node(
        ossia_device_t,
        const char *,
        ossia_node_t*);

/**
 * @brief ossia_devices_get_child
 * @param An existing root node
 * @param A name
 * @param A node receiving the root's child with the specified name if it exists
 * @return A result code
 */

ossia_preset_result ossia_devices_get_child(
        ossia_node_t,
        const char*,
        ossia_node_t*);

/**
 * @brief ossia_free_string
 * @param A string with allocated memory
 * @return A result code
 *
 * Frees the string passed as parameter
 */

ossia_preset_result ossia_free_string(
        const char*);

ossia_preset_result ossia_devices_get_node_address(
        ossia_node_t,
        ossia_address_t*);

#if defined(__cplusplus)
}
#endif

#endif
