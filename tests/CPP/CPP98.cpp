// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia-cpp/ossia-cpp98.hpp>
#include <iostream>
static void test_callback(void* n, const opp::value& v)
{
  int* count = reinterpret_cast<int*>(n);
  *count += 1;
  std::cout << "Received a list: " << v.is_list() << ": "
            << v.to_string() << std::endl;
}

TEST_CASE ("basic", "[basic]")
{
  using namespace std::literals;
  opp::oscquery_server server{"banana"};

  auto root = server.get_root_node();
  auto argb = root
      .create_child("foo")
      .create_argb("bar/baz")
      .set_access(opp::Get)
      ;

  // creates /foo/bar/baz of type argb in get
  REQUIRE(argb.get_address() == "/foo/bar/baz"s);
  REQUIRE(argb.get_unit() == "color.argb"s);

  auto blob = root.create_buffer("/foo/my/buffer");
  REQUIRE(blob.get_value().is_string());
}

TEST_CASE ("multi", "[multi]")
{
  REQUIRE_NOTHROW([] {
    opp::oscquery_server d1("supersoftware", 1234, 5678);
    opp::oscquery_server d2("supersoftware", 1234, 5678);
  }());
}

TEST_CASE ("preset", "[preset]")
{
  {
    opp::oscquery_server server;
    opp::node root = server.get_root_node();

    // Create a node /foo (just a container without parameter)
    opp::node n1 = root.create_child("foo");

    // Create a node /foo/bar/baz of type float
    // and make it read-only on the network.

    opp::node n2 = n1.create_float("bar/baz")
        .set_access(opp::Get)
        .set_bounding(opp::Clip);
    n2.set_min(0.5);
    n2.set_max(0.7);
    n2.set_value(0.6);

    // This can be decomposed in several steps:
    // Create an empty node
    opp::node n3;
    // give it a parent and a name
    n3 = root.create_child("boo");
    // Then add a float parameter to it:
    n3.set_float();
    // Add an unit to the parameter:
    n3.set_unit("gain.db");
    n3.set_value(-12.56);

    auto n4 = n1.create_argb("my_color");
    n4.set_value(opp::value::vec4f{0.1f,0.2f,0.3f,0.4f});

    auto n5 = n1.create_vec2f("my_vec2f");
    n5.set_value(opp::value::vec2f{0.5f,0.6f});

    auto n6 = n1.create_vec3f("my_vec3f");
    n6.set_value(opp::value::vec3f{0.56f,0.66f,0.76f});

    auto n7 = n1.create_vec4f("my_vec4f");
    n7.set_value(opp::value::vec4f{0.57f,0.67f,0.77f,0.87f});

    auto n8 = n1.create_list("my_list");
    std::vector<opp::value> list;
    list.push_back(3);
    list.push_back(2.68f);
    list.push_back("toto");
    list.push_back('a');
    std::vector<opp::value> sublist;
    sublist.push_back("foo");
    sublist.push_back("bar");
    sublist.push_back(36.25);
    list.push_back(sublist);
    n8.set_value(list);

    REQUIRE(n8.get_value().is_list());
    auto list2 = n8.get_value().to_list();
    REQUIRE(list2.size() == (size_t)5);

    auto n9 = n1.create_int("my_int");
    n9.set_value(99);

    auto n10 = n1.create_bool("my_bool");
    n10.set_value(true);

    auto n11 = n1.create_string("my_string");
    n11.set_value("truc muche");

    auto n12 = n1.create_char("my_char");
    n12.set_value('e');

    root.save_preset("test.json");
  }

  {
    opp::oscquery_server server;
    opp::node root = server.get_root_node();

    // recreate the tree and try to apply preset

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

    auto n4 = n1.create_argb("my_color");
    auto n5 = n1.create_vec2f("my_vec2f");
    auto n6 = n1.create_vec3f("my_vec3f");
    auto n7 = n1.create_vec4f("my_vec4f");
    auto n8 = n1.create_list("my_list");
    auto n9 = n1.create_int("my_int");
    auto n10 = n1.create_bool("my_bool");
    auto n11 = n1.create_string("my_string");
    auto n12 = n1.create_char("my_char");

    root.load_preset("test.json");


    REQUIRE(n2.get_value().to_float() == 0.6f);

    REQUIRE(n3.get_value().to_float() == -12.56f);

    REQUIRE(n4.get_value().to_vec4f()[0] == 0.1f);
    REQUIRE(n4.get_value().to_vec4f()[1] == 0.2f);
    REQUIRE(n4.get_value().to_vec4f()[2] == 0.3f);
    REQUIRE(n4.get_value().to_vec4f()[3] == 0.4f);

    REQUIRE(n5.get_value().to_vec2f()[0] == 0.5f);
    REQUIRE(n5.get_value().to_vec2f()[1] == 0.6f);

    REQUIRE(n6.get_value().to_vec4f()[0] == 0.56f);
    REQUIRE(n6.get_value().to_vec4f()[1] == 0.66f);
    REQUIRE(n6.get_value().to_vec4f()[2] == 0.76f);

    REQUIRE(n7.get_value().to_vec4f()[0] == 0.57f);
    REQUIRE(n7.get_value().to_vec4f()[1] == 0.67f);
    REQUIRE(n7.get_value().to_vec4f()[2] == 0.77f);
    REQUIRE(n7.get_value().to_vec4f()[3] == 0.87f);

    REQUIRE(n8.get_value().is_list());
    auto list = n8.get_value().to_list();
    REQUIRE(list.size() == (size_t)5);
    REQUIRE(n8.get_value().to_list()[0].to_int() == 3);
    REQUIRE(n8.get_value().to_list()[1].to_float() == 2.68f);
    REQUIRE(n8.get_value().to_list()[2].to_string() == std::string("toto"));
    REQUIRE(n8.get_value().to_list()[3].to_char() == 'a');
    REQUIRE(n8.get_value().to_list()[4].to_list()[0].to_string() == std::string("foo"));
    REQUIRE(n8.get_value().to_list()[4].to_list()[1].to_string() == std::string("bar"));
    REQUIRE(n8.get_value().to_list()[4].to_list()[2].to_float() == 36.25);

    REQUIRE(n9.get_value().to_int() == 99);

    REQUIRE(n10.get_value().to_bool() == true);

    REQUIRE(n11.get_value().to_string() == std::string("truc muche"));

    REQUIRE(n12.get_value().to_char() == 'e');
  }

}

TEST_CASE ("various", "[various]")
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

  opp::node n4 = root.create_list("empty_list");

  opp::node n5 = root.create_list("list");
  std::vector<opp::value> val;
  opp::value::vec2f my_vec2f{0.1f,0.2f};
  val.push_back(my_vec2f);
  n5.set_value(val);

  {
    REQUIRE(n5.get_value().is_list());
    auto my_list = n5.get_value().to_list();
    REQUIRE(my_list.size() == 1);
    REQUIRE(my_list[0].is_vec2f());
    auto my_remote_vec = my_list[0].to_vec2f();
    CHECK(my_remote_vec[0] == 0.1f);
    CHECK(my_remote_vec[1] == 0.2f);
  }

  ////////////////////////////////////////////////////////////////////////
  //// Step 2. Creating another device to connect with the first one. ////
  ////////////////////////////////////////////////////////////////////////


  opp::oscquery_mirror remote_dev("remote", "ws://127.0.0.1:5678");

  // Request an update of the device.
  remote_dev.refresh();

  // We expect to find remote_n2 here, but in the general case
  // network problems can happen so check the returned value.
  opp::node remote_n2 = remote_dev.get_root_node().find_child("/foo/bar/baz");
  REQUIRE(remote_n2);

  // After some time n2 wil get the value that we send here:
  remote_n2.set_value(0.1);
  // It will however be filtered according to the domain we set, e.g.
  // the result will be argb(0.5, 0.2, 0.5, 0.9).
  remote_n2.set_value(0.8);


  opp::node remote_n4 = remote_dev.get_root_node().find_child("/empty_list");
  REQUIRE(remote_n4);

  REQUIRE(remote_n4.get_value().is_list());
  REQUIRE(remote_n4.get_value().to_list().empty());

  opp::node remote_n5 = remote_dev.get_root_node().find_child("/list");
  REQUIRE(remote_n5);

  REQUIRE(remote_n5.get_value().is_list());
  auto my_list = remote_n5.get_value().to_list();
  REQUIRE(my_list.size() == 1);
  bool flag = (my_list[0].is_vec2f() || (my_list[0].is_list() && my_list[0].to_list().size() == 2));
  CHECK(flag);
  if(my_list[0].is_vec2f())
  {
    auto my_remote_vec = my_list[0].to_vec2f();
    CHECK(my_remote_vec[0] == 0.1f);
    CHECK(my_remote_vec[1] == 0.2f);
  }
  // this is a limitation (or should I say a bug ??) of oscquery specs
  // list element are not typed, so a list of vec2f look the same as a list of list of float
  else if ( my_list[0].is_list() )
  {
    auto my_remote_nested_list = my_list[0].to_list();
    CHECK(my_remote_nested_list[0].to_float() == 0.1f);
    CHECK(my_remote_nested_list[1].to_float() == 0.2f);
  }
  //////////////////////////////////////////////////////
  //// Step 3. Receiving changes through callbacks. ////
  //////////////////////////////////////////////////////

  struct {
    volatile int count;
  } cnt;
  cnt.count = 0;
  remote_n2.set_value_callback(test_callback, &cnt);

  n2.set_value(0.5);
  n2.set_value(0.9);
  while(cnt.count < 2)
    ;


  // Node deletion:

  // Method 1 :
  //root.remove_children();

  // Method 2 :
  if (n1)root.remove_child(n1.get_name());
  if (n2)root.remove_child(n2.get_name());
  if (n3)root.remove_child(n3.get_name());
}
