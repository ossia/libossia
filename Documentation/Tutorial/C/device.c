#include <ossia-c/ossia-c.h>

/* The C API wraps the C++14 API and follows the same concepts. */
int main(int argc, char** argv)
{
  /** ~ Welcome to the libossia C89 tutorial! ~ **/

  /*************************************************/
  /** Step 1. Creating a device with a few nodes. **/
  /*************************************************/
  /* Setup our communication protocol */
  ossia_protocol_t proto = ossia_protocol_oscquery_server_create(1234, 5678);

  /* Our device */
  ossia_device_t dev = ossia_device_create(proto, "supersoftware");

  /* Get the root node of the device tree */
  ossia_node_t root = ossia_device_get_root_node(dev);

  /* Create a sub-child on it */
  ossia_node_t my = ossia_node_create(root, "/foo/baz");

  /* This sub-child, /bort/bert, will have a float value. */
  ossia_address_t addr = ossia_node_create_address(my, ossia_type::FLOAT_T);

  /* Send a value through the network */
  ossia_address_push_f(addr, 345.);

  /********************************************************************/
  /** Step 2. Creating another device to connect with the first one. **/
  /********************************************************************/
  ossia_protocol_t remote_proto = ossia_protocol_oscquery_mirror_create("ws://127.0.0.1:5678");
  ossia_device_t remote_dev = ossia_device_create(remote_proto, "supersoftware");
  ossia_node_t remote_root = ossia_device_get_root_node(dev);

  /* Request an update of the root node.
   * dev will send all the information of the tree to remote_dev. */
  ossia_device_update_namespace(remote_dev);

  /* We expect to find remote_n2 here, but in the general case
   * network problems can happen so check the returned pointer. */
  ossia_node_t remote_n2 = ossia_node_find(remote_root, "/foo/baz");
  if(!remote_n2)
    return -1;

  /* Enjoy manual memory management. */
  ossia_device_free(dev);
}
