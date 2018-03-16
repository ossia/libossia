#include <ossia-cpp/ossia-cpp98.hpp>
#include <iostream>

void test_callback(void* n, const opp::value& v)
{
  int* count = reinterpret_cast<int*>(n);
  *count += 1;
  std::cout << "Received a list: " << v.is_list() << ": "
            << v.to_string() << std::endl;
}

int main()
{
  /** ~ Welcome to the libossia C++98 tutorial! ~ **/

  // This is a slightly higher-level API than the C++14 API.
  // It is also compatible with older compilers
  // and does not require any special library to be used
  // (though ABI problems may arise on some platforms).
  // It is hard to misuse, however of course performance may take a hit.


  /////////////////////////////////////////////////////
  //// Step 1. Creating a device with a few nodes. ////
  /////////////////////////////////////////////////////


  // Create a device
  opp::oscquery_server dev("supersoftware");
  opp::node root = dev.get_root_node();

  // Create a node /foo
  opp::node n1 = root.create_child("foo");

  // Create a node /foo/bar/baz of type float
  // and make it read-only on the network.
  opp::node n2 = n1.create_float("bar/baz")
                   .set_access(opp::Get)
                   .set_bounding(opp::Clip);
  n2.set_min(0.5);
  n2.set_max(0.7);

  // Create a node /boo (just a container without parameter)
  opp::node n3;
  n3 = root.create_child("boo");


  ////////////////////////////////////////////////////////////////////////
  //// Step 2. Creating another device to connect with the first one. ////
  ////////////////////////////////////////////////////////////////////////


  opp::oscquery_mirror remote_dev("remote", "ws://127.0.0.1:5678");

  // Request an update of the device.
  remote_dev.refresh();

  // We expect to find remote_n2 here, but in the general case
  // network problems can happen so check the returned value.
  opp::node remote_n2 = remote_dev.get_root_node().find_child("/foo/bar/baz");
  if(!remote_n2.valid())
    return 1;

  // After some time n2 wil get the value that we send here:
  remote_n2.set_value(0.1);
  // It will however be filtered according to the domain we set, e.g.
  // the result will be argb(0.5, 0.2, 0.5, 0.9).


  //////////////////////////////////////////////////////
  //// Step 3. Receiving changes through callbacks. ////
  //////////////////////////////////////////////////////


  int count = 0;
  remote_n2.set_value_callback(test_callback, &count);

  n2.set_value(0.5);
  n2.set_value(0.6);
  while(count < 2)
    ;

  // Trying to delete nodes - all methods crash (in different ways)

    // Method 1 :
  //std::vector<opp::node> children_list;
  //children_list = root.get_children();
  //for (auto child : children_list)
  //     root.remove_child(child.get_name());

  // Method 2 :
  //root.remove_children();

  // Method 3 :
  //if (n1.valid())root.remove_child(n1.get_name());
  //if (n2.valid())root.remove_child(n2.get_name());
  if (n3.valid())root.remove_child(n3.get_name());


  return 0;
}
