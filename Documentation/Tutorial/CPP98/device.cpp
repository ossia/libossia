#include <ossia-cpp/ossia-cpp98.hpp>

int main()
{
  /** ~ Welcome to the libossia C++98 tutorial! ~ **/

  /////////////////////////////////////////////////////
  //// Step 1. Creating a device with a few nodes. ////
  /////////////////////////////////////////////////////
  // This is a slightly higher-level API than the C++14 API.
  // It is also compatible with older compilers
  // and does not require any special library to be used
  // (though ABI problems may arise on some platforms).
  // It is hard to misuse, however of course performance may take a hit.

  // Create a device
  opp::oscquery_server dev{"supersoftware"};
  opp::node root = dev.get_root_node();

  // Create a node /foo
  opp::node foo = root.create_child("foo");

  // Create a node /foo/bar/baz of type ARGB color
  // and make it read-only on the network.
  opp::node col = foo.create_argb("bar/baz")
                     .set_access(opp::Get)
                     .set_bounding(opp::bounding_mode::Clip);
  col.set_min(std::vector<opp::value>(0.5, 0.2, 0.2, 0.2));
  col.set_max(std::vector<opp::value>(0.7, 0.8, 0.8, 0.8));

  ////////////////////////////////////////////////////////////////////////
  //// Step 2. Creating another device to connect with the first one. ////
  ////////////////////////////////////////////////////////////////////////
  opp::oscquery_mirror remote_dev{"supersoftware", "ws://127.0.0.1:5678"};

  // Request an update of the device.
  remote_dev.refresh();

  // We expect to find remote_n2 here, but in the general case
  // network problems can happen so check the returned value.
  opp::node remote_n2 = remote_dev.get_root_node().find_child("/foo/bar/baz");
  if(!remote_n2.valid())
    return 1;

  // After some time "col" wil get the value that we send here:
  remote_n2.set_value(std::vector<opp::value>(0.1, 0.1, 0.5, 0.9));
  // It will however be filtered according to the domain we set, e.g.
  // the result will be argb(0.5, 0.2, 0.5, 0.9).
}
