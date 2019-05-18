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
  opp::oscquery_server dev("supersoftware", 1234, 5678);
  opp::node root = dev.get_root_node();

  // Create a node /foo (just a container without parameter)
  opp::node n1 = root.create_child("foo");

  // Create a node /foo/bar/baz of type float
  // and make it read-only on the network.
  opp::node n2 = n1.create_float("bar/baz")
                   .set_access(opp::Get)
                   .set_bounding(opp::Clip);
  n2.set_min(0.5);
  n2.set_max(0.7);

  // This can be decomposed in several steps:
  // Create an empty node
  opp::node n3;
  // give it a parent and a name
  n3 = root.create_child("boo");
  // Then add a float parameter to it:
  n3.set_float();
  // Add an unit to the parameter:
  n3.set_unit("gain.db");


  ////////////////////////////////////////////////////////////////////////
  //// Step 2. Creating another device to connect with the first one. ////
  ////////////////////////////////////////////////////////////////////////

  {
    opp::oscquery_mirror remote_dev("remote", "ws://127.0.0.1:5678");
    // Request an update of the device.
    remote_dev.refresh();

    // We expect to find remote_n2 here, but in the general case
    // network problems can happen so check the returned value.
    opp::node remote_n2 = remote_dev.get_root_node().find_child("/foo/bar/baz");
    if(!remote_n2)
      return 1;

    // After some time n2 wil get the value that we send here:
    remote_n2.set_value(0.1);
    // It will however be filtered according to the domain we set, e.g.
    // the result will be argb(0.5, 0.2, 0.5, 0.9).
    remote_n2.set_value(0.8);

    //////////////////////////////////////////////////////
    //// Step 3. Receiving changes through callbacks. ////
    //////////////////////////////////////////////////////


    int count = 0;
    remote_n2.set_value_callback(test_callback, &count);

    n2.set_value(0.5);
    n2.set_value(0.9);
    while(count < 2)
      ;
  }


  // Node deletion:

  // Method 1 :
  //root.remove_children();

  // Method 2 :
  if (n1) root.remove_child(n1.get_name());
  // n2 and n3 are child of n1 so they are removed too

  return 0;
}
