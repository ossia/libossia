#include <ossia-c/ossia-c.h>
#include <stdio.h>

void test_callback(void* n, ossia_value_t v)
{
  int* count = (int*)n;
  *count += 1;
  char* str;
  size_t sz;
  ossia_value_convert_byte_array(v, &str, &sz);
  printf("Received: %s\n", str);
  ossia_string_free(str);
  ossia_value_free(v);
}

int main(int argc, char** argv)
{
  /** ~ Welcome to the libossia C89 tutorial! ~ **/

  /* The C API wraps the C++14 API and follows the same concepts. */


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
  ossia_node_t n1 = ossia_node_create(root, "/foo/blu");
  ossia_node_t n2 = ossia_node_create(root, "/foo/baz");

  /* This sub-child, /bort/bert, will have a float value. */
  ossia_parameter_t n2_addr = ossia_node_create_parameter(n2, FLOAT_T);

  /* Send a value through the network */
  ossia_parameter_push_f(n2_addr, 345.);


  /********************************************************************/
  /** Step 2. Creating another device to connect with the first one. **/
  /********************************************************************/


  ossia_protocol_t remote_proto = ossia_protocol_oscquery_mirror_create("ws://127.0.0.1:5678");
  ossia_device_t remote_dev = ossia_device_create(remote_proto, "remote");
  ossia_node_t remote_root = ossia_device_get_root_node(dev);

  /* Request an update of the root node.
   * dev will send all the information of the tree to remote_dev. */
  ossia_device_update_namespace(remote_dev);

  /* We expect to find remote_n2 here, but in the general case
   * network problems can happen so check the returned pointer. */
  ossia_node_t remote_n2 = ossia_node_find(remote_root, "/foo/baz");
  if(!remote_n2)
    return -1;


  /**************************************************/
  /** Step 3. Receiving changes through callbacks. **/
  /**************************************************/


  ossia_parameter_t remote_n2_addr = ossia_node_get_parameter(remote_n2);

  int count = 0;
  ossia_parameter_push_callback(remote_n2_addr, test_callback, &count);

  ossia_parameter_push_f(n2_addr, 3.4);
  ossia_parameter_push_f(n2_addr, 5.6);
  while(count != 2)
    ;

  /* Enjoy manual memory management. */
  ossia_protocol_free(remote_proto);
  ossia_device_free(remote_dev);

  ossia_protocol_free(proto);
  ossia_device_free(dev);
}
