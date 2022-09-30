#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>

int main()
{
  //// ~ Welcome to the libossia C++14 tutorial! ~ ////

  // This is the "raw" libossia API written in modern C++.
  // It can be used to make your applications without any compromises
  // in performance.


  /////////////////////////////////////////////////////
  //// Step 1. Creating a device with a few nodes. ////
  /////////////////////////////////////////////////////


  using namespace ossia;
  using namespace ossia::net;

  // The class generic_device represents a tree of parameters that
  // you can build to match your application.
  generic_device dev{
    std::make_unique<ossia::oscquery::oscquery_server_protocol>(),
    "supersoftware"};

  // Create some nodes in the tree. The device can be used as root.
  auto& n1 = create_node(dev, "/foo/bar");
  auto& n2 = create_node(n1, "baz");

  // A parameter allows storing values & such.
  auto addr = n2.create_parameter(val_type::FLOAT);
  addr->set_domain(make_domain(-10, 10));

  // The value will be send over the network.
  addr->push_value(1.f);


  ////////////////////////////////////////////////////////////////////////
  //// Step 2. Creating another device to connect with the first one. ////
  ////////////////////////////////////////////////////////////////////////


  generic_device remote_dev{
    std::make_unique<ossia::oscquery::oscquery_mirror_protocol>("ws://127.0.0.1:5678"),
    "remote"};

  // Request an update of the root node.
  // dev will send all the information of the tree to remote_dev.
  remote_dev.get_protocol().update(remote_dev);

  // We expect to find remote_n2 here, but in the general case
  // network problems can happen so check the returned pointer.
  auto remote_n2 = find_node(remote_dev, "/foo/bar/baz");
  if(!remote_n2)
    return 1;

  // After some time "n2" will get the value that we send here:
  remote_n2->get_parameter()->push_value(6.);


  //////////////////////////////////////////////////////
  //// Step 3. Receiving changes through callbacks. ////
  //////////////////////////////////////////////////////


  int count = 0;
  remote_n2->get_parameter()->add_callback([&] (const auto& value) {
    std::cerr << ossia::convert<std::string>(value) << std::endl;
    ++count;
  });

  n2.get_parameter()->push_value(123);
  n2.get_parameter()->push_value(3456);

  while(count != 2)
    ;
}
