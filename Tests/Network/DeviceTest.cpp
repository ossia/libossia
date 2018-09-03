// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <iostream>
#include <ossia/network/local/local.hpp>

#if defined(OSSIA_QT)
#include <QTimer>
#include <QCoreApplication>
#endif
#if defined(OSSIA_PROTOCOL_WEBSOCKETS)
#include <ossia-qt/websocket-generic-client/ws_generic_client_protocol.hpp>
#endif
#if defined(OSSIA_PROTOCOL_HTTP)
#include <ossia-qt/http/http_protocol.hpp>
#endif
#if defined(OSSIA_PROTOCOL_OSC)
#include <ossia/network/osc/osc.hpp>
#endif
#if defined(OSSIA_PROTOCOL_MIDI)
#include <ossia/network/midi/midi.hpp>
#endif
#if defined(OSSIA_PROTOCOL_MINUIT)
#include <ossia/network/minuit/minuit.hpp>
#endif
#if defined(OSSIA_PROTOCOL_OSCQUERY)
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#endif
#include <ossia/network/common/parameter_properties.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include "TestUtils.hpp"

using namespace ossia;
// TODO move this code where it is useful.
template<ossia::val_type>
struct matching_domain;
template<>
struct matching_domain<ossia::val_type::IMPULSE>
{ using type = ossia::domain_base<impulse>; };
template<>
struct matching_domain<ossia::val_type::BOOL>
{ using type = ossia::domain_base<bool>; };
template<>
struct matching_domain<ossia::val_type::INT>
{ using type = ossia::domain_base<int>; };
template<>
struct matching_domain<ossia::val_type::FLOAT>
{ using type = ossia::domain_base<float>; };
template<>
struct matching_domain<ossia::val_type::CHAR>
{ using type = ossia::domain_base<char>; };
template<>
struct matching_domain<ossia::val_type::STRING>
{ using type = ossia::domain_base<std::string>; };
template<>
struct matching_domain<ossia::val_type::LIST>
{ using type = ossia::vector_domain; };
template<>
struct matching_domain<ossia::val_type::VEC2F>
{ using type = ossia::vecf_domain<2>; };
template<>
struct matching_domain<ossia::val_type::VEC3F>
{ using type = ossia::vecf_domain<3>; };
template<>
struct matching_domain<ossia::val_type::VEC4F>
{ using type = ossia::vecf_domain<4>; };


struct remote_data
{
  template<typename FunProto1, typename FunProto2>
  remote_data(
      FunProto1 local_proto,
      FunProto2 remote_proto):
    local_device{local_proto(), "score" },
    remote_device{remote_proto(), "score-remote"}
  {
    int N = 10;

    for(int i = 0; i < N; i++)
    {
      auto cld = local_device.create_child(std::to_string(i));
      local_addr.push_back(cld->create_parameter((ossia::val_type) i));
    }

    for(int i = 0; i < N; i++)
    {
      auto cld = remote_device.create_child(std::to_string(i));
      remote_addr.push_back(cld->create_parameter((ossia::val_type) i));
    }
  }

  ossia::net::generic_device local_device;
  ossia::net::generic_device remote_device;

  std::vector<ossia::net::parameter_base*> local_addr;
  std::vector<ossia::net::parameter_base*> remote_addr;
};



const std::vector<ossia::value> value_to_test{
  ossia::impulse{},
  int32_t{0},
  int32_t{-1000},
  int32_t{1000},
  float{0},
  float{-1000},
  float{1000},
  bool{true},
  bool{false},
  char{},
  char{'a'},
  std::string{""},
  std::string{"ossia"},
  std::vector<ossia::value>{},
  std::vector<ossia::value>{int32_t{0}},
  std::vector<ossia::value>{int32_t{0}, int32_t{1}},
  std::vector<ossia::value>{float{0}, int32_t{1}},
  std::vector<ossia::value>{float{0}, int32_t{1}, std::string{}, ossia::impulse{}},
  std::vector<ossia::value>{float{0}, float{1000}},
  ossia::vec2f{},
  ossia::vec3f{},
  ossia::vec4f{}
};

template<typename FunProto1, typename FunProto2>
void test_comm_generic(
    FunProto1 local_proto,
    FunProto2 remote_proto)
{
  int N = 10;
  remote_data rem{std::move(local_proto), std::move(remote_proto)};
  auto& local_addr = rem.local_addr;
  auto& remote_addr = rem.remote_addr;

  auto push_all_values = [&] {
    for(int i = 0; i < N; i++)
    {
      for(const ossia::value& val : value_to_test)
      {
        local_addr[i]->set_value_type(val.get_type());
        local_addr[i]->push_value(val);
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    for(int i = 0; i < N; i++)
    {
      local_addr[i]->set_value_type((ossia::val_type) i);
      local_addr[i]->push_value(ossia::init_value((ossia::val_type) i));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    for(int i = 0; i < N; i++)
    {
      for(const ossia::value& val : value_to_test)
      {
        remote_addr[i]->set_value_type(val.get_type());
        remote_addr[i]->push_value(val);
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    for(int i = 0; i < N; i++)
    {
      remote_addr[i]->set_value_type((ossia::val_type) i);
      remote_addr[i]->push_value(ossia::init_value((ossia::val_type) i));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  };

  push_all_values();


  for(auto val : local_addr)
  {
    rem.local_device.get_protocol().observe(*val, true);
  }
  for(auto val : remote_addr)
  {
    rem.remote_device.get_protocol().observe(*val, true);
  }
  push_all_values();


  auto test_all_values = [&] (std::vector<ossia::net::parameter_base*>& vec)
  {
    for(int access_i = 0; access_i < 3; access_i++)
    {
      for(int i = 0; i < N; i++)
      {
        vec[i]->set_access(access_mode(access_i));
      }
      push_all_values();
      for(int bounding_i = 0 ; bounding_i < 6; bounding_i++)
      {
        for(int i = 0; i < N; i++)
        {
          vec[i]->set_bounding(bounding_mode(bounding_i));
        }
        push_all_values();

        for(int domain_i = 0; domain_i < N; domain_i++)
        {
          vec[domain_i]->set_domain(init_domain(ossia::val_type(domain_i)));
        }
        push_all_values();


        for(int domain_i = 0; domain_i < N; domain_i++)
        {
          auto val = ossia::init_value((ossia::val_type) domain_i);
          auto dom = ossia::make_domain(val, val);
          vec[domain_i]->set_domain(dom);
        }
        push_all_values();

      }
    }
  };

  test_all_values(local_addr);

  for(int i = 0; i < 10; i++)
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

  test_all_values(remote_addr);

  for(int i = 0; i < 10; i++)
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

}




#if defined(OSSIA_PROTOCOL_OSC)
TEST_CASE ("test_bundle", "test_bundle")
    {
        ossia::net::generic_device osc_A{
          std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997), "test_osc"};
        ossia::net::generic_device osc_B{
          std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9997, 9996), "test_osc"};

        auto a1 = ossia::net::create_node(osc_A, "/foo/bar.0").create_parameter(ossia::val_type::FLOAT);
        auto a2 = ossia::net::create_node(osc_A, "/foo/bar.1").create_parameter(ossia::val_type::FLOAT);
        auto a3 = ossia::net::create_node(osc_A, "/foo/bar.2").create_parameter(ossia::val_type::FLOAT);
        auto a4 = ossia::net::create_node(osc_A, "/foo/bar.3").create_parameter(ossia::val_type::FLOAT);

        auto b1 = ossia::net::create_node(osc_B, "/foo/bar.0").create_parameter(ossia::val_type::FLOAT);
        auto b2 = ossia::net::create_node(osc_B, "/foo/bar.1").create_parameter(ossia::val_type::FLOAT);
        auto b3 = ossia::net::create_node(osc_B, "/foo/bar.2").create_parameter(ossia::val_type::FLOAT);
        auto b4 = ossia::net::create_node(osc_B, "/foo/bar.3").create_parameter(ossia::val_type::FLOAT);

        for(auto b : {b1, b2, b3, b4})
        {
          b->set_value(1234.);
        }

        auto& b_proto = (ossia::net::osc_protocol&) osc_B.get_protocol();

        b_proto.push_bundle({b1, b2, b3, b4});

        std::this_thread::sleep_for(std::chrono::microseconds(10000));
        REQUIRE(a1->value() == ossia::value{1234.});
        REQUIRE(a2->value() == ossia::value{1234.});
        REQUIRE(a3->value() == ossia::value{1234.});
        REQUIRE(a4->value() == ossia::value{1234.});
    }

TEST_CASE ("test_bundle_raw", "test_bundle_raw")
    {
        ossia::net::generic_device osc_A{
          std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997), "test_osc"};
        ossia::net::generic_device osc_B{
          std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9997, 9996), "test_osc"};

        auto a1 = ossia::net::create_node(osc_A, "/foo/bar.0").create_parameter(ossia::val_type::FLOAT);
        auto a2 = ossia::net::create_node(osc_A, "/foo/bar.1").create_parameter(ossia::val_type::FLOAT);
        auto a3 = ossia::net::create_node(osc_A, "/foo/bar.2").create_parameter(ossia::val_type::FLOAT);
        auto a4 = ossia::net::create_node(osc_A, "/foo/bar.3").create_parameter(ossia::val_type::FLOAT);

        int recv = 0;
        a1->add_callback([&] (const auto& v) { recv++; });
        a2->add_callback([&] (const auto& v) { recv++; });
        a3->add_callback([&] (const auto& v) { recv++; });
        a4->add_callback([&] (const auto& v) { recv++; });
        auto& b_proto = (ossia::net::osc_protocol&) osc_B.get_protocol();

        std::vector<ossia::net::full_parameter_data> vec(4);
        vec[0].address = "/foo/bar.0";
        vec[0].set_value(0.);
        vec[1].address = "/foo/bar.1";
        vec[1].set_value(1.);
        vec[2].address = "/foo/bar.2";
        vec[2].set_value(2.);
        vec[3].address = "/foo/bar.3";
        vec[3].set_value(3.);

        b_proto.push_raw_bundle(vec);

        while(recv < 4)
          std::this_thread::sleep_for(std::chrono::milliseconds(10));

        REQUIRE(a1->value() == ossia::value{0.});
        REQUIRE(a2->value() == ossia::value{1.});
        REQUIRE(a3->value() == ossia::value{2.});
        REQUIRE(a4->value() == ossia::value{3.});
    }

TEST_CASE ("test_pattern_match", "test_pattern_match")
    {
        ossia::net::generic_device osc_A{
          std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997), "test_osc"};
        ossia::net::generic_device osc_B{
          std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9997, 9996), "test_osc"};

        auto a1 = ossia::net::create_node(osc_A, "/foo/bar.0").create_parameter(ossia::val_type::FLOAT);
        auto a2 = ossia::net::create_node(osc_A, "/foo/bar.1").create_parameter(ossia::val_type::FLOAT);
        auto a3 = ossia::net::create_node(osc_A, "/foo/bar.2").create_parameter(ossia::val_type::FLOAT);
        auto a4 = ossia::net::create_node(osc_A, "/foo/bar.3").create_parameter(ossia::val_type::FLOAT);

        int recv = 0;
        a1->add_callback([&] (const auto& v) { recv++; });
        a2->add_callback([&] (const auto& v) { recv++; });
        a3->add_callback([&] (const auto& v) { recv++; });
        a4->add_callback([&] (const auto& v) { recv++; });
        auto& b_proto = (ossia::net::osc_protocol&) osc_B.get_protocol();

        ossia::net::full_parameter_data dat{"/foo/bar.*"};
        dat.set_value(2.3f);
        b_proto.push_raw(dat);

        while(recv < 4)
          std::this_thread::sleep_for(std::chrono::milliseconds(10));

        REQUIRE(a1->value() == ossia::value{2.3});
        REQUIRE(a2->value() == ossia::value{2.3});
        REQUIRE(a3->value() == ossia::value{2.3});
        REQUIRE(a4->value() == ossia::value{2.3});
    }
#endif


  /*! test life cycle and accessors functions */
TEST_CASE ("test_basic", "test_basic")
  {
    {
      ossia::net::generic_device local_device{"test" };
    }
    {
#if defined(OSSIA_PROTOCOL_OSC)
      ossia::net::generic_device osc_device{
        std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997), "test_osc" };
#endif
    }
    {
#if defined(OSSIA_PROTOCOL_MINUIT)
      ossia::net::generic_device minuit_device{
        std::make_unique<ossia::net::minuit_protocol>("score", "127.0.0.1", 13579, 13580), "test_minuit" };
#endif
    }
  }

#if defined(OSSIA_PROTOCOL_OSCQUERY)
TEST_CASE ("test_oscquery_multi", "test_oscquery_multi")
  {
    try {

    ossia::net::generic_device device1{std::make_unique<ossia::oscquery::oscquery_server_protocol>(1234, 4567), "x"};
    ossia::net::generic_device device2{std::make_unique<ossia::oscquery::oscquery_server_protocol>(1234, 4567), "y"};
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } catch (const std::exception& e) {

    }
  }
#endif

TEST_CASE ("test_midi", "test_midi")
  {
    using namespace ossia::net::midi;
    try {
      auto proto = std::make_unique<midi_protocol>();
      auto res = proto->scan();
      for(auto e : res)
      {
        ossia::net::midi::midi_device dev(std::make_unique<midi_protocol>(e));
        dev.set_name("dada");
        dev.update_namespace();
      }

    }
    catch(...)
    {
    }
  }

#if defined(OSSIA_PROTOCOL_OSC)
TEST_CASE ("test_comm_osc", "test_comm_osc")
  {
    test_comm_generic([] { return std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997); },
                      [] { return std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9997, 9996); });
  }
#endif

#if defined(OSSIA_PROTOCOL_MINUIT)
TEST_CASE ("test_comm_minuit", "test_comm_minuit")
  {
    test_comm_generic([] { return std::make_unique<ossia::net::minuit_protocol>("score-remote", "127.0.0.1", 13579, 13580); },
                      [] { return std::make_unique<ossia::net::minuit_protocol>("score-remote", "127.0.0.1", 13580, 13579); });


    int N = 10;
    auto proto = std::make_unique<ossia::net::multiplex_protocol>();
    auto proto_p = proto.get();
    ossia::net::generic_device local_device{std::move(proto), "score"};

    for(int i = 0; i < N; i++)
    {
      auto cld = local_device.create_child(std::to_string(i));
      cld->create_parameter((ossia::val_type) i);
    }

    proto_p->expose_to(
          std::make_unique<ossia::net::minuit_protocol>("score-remote", "127.0.0.1", 13579, 13580));

    ossia::net::generic_device remote_device{
      std::make_unique<ossia::net::minuit_protocol>("score-remote", "127.0.0.1", 13580, 13579), "score-remote"};
    remote_device.get_protocol().update(remote_device);

    for(auto n : remote_device.children_copy())
    {
      if(auto a = n->get_parameter())
      {
        a->pull_value();
      }
    }
  }
#endif

#if defined(OSSIA_PROTOCOL_OSCQUERY)
TEST_CASE ("test_comm_oscquery", "test_comm_oscquery")
  {
    try {
      test_comm_generic([] { return std::make_unique<ossia::oscquery::oscquery_server_protocol>(1234, 5678); },
                        [] { return std::make_unique<ossia::oscquery::oscquery_mirror_protocol>("ws://127.0.0.1:5678"); });
    }
    catch(const std::exception& e) {
      std::cerr << "Error : " << e.what();
      REQUIRE(false);
    }
  }
#endif

#if defined(OSSIA_PROTOCOL_HTTP)
TEST_CASE ("test_http", "test_http")
  {
    int argc{}; char** argv{};
    QCoreApplication app(argc, argv);

    ossia::context context;

    QFile f("testdata/http/http_example.qml");
    f.open(QFile::ReadOnly);

    ossia::net::http_device http_device{
      std::make_unique<ossia::net::http_protocol>(
            f.readAll()),
          "test" };

    // For the sake of coverage...
    const auto& const_dev = http_device;
    REQUIRE(&http_device.get_root_node() == &const_dev.get_root_node());

    // We have to wait a bit for the event loop to run.
    QTimer t;
    QObject::connect(&t, &QTimer::timeout, [&] () {
      auto node = ossia::net::find_node(http_device, "/tata/tutu");
      if(node)
      {
        node->get_parameter()->push_value(ossia::impulse{});
      }
    });
    t.setInterval(1000);
    t.setSingleShot(true);
    t.start();

    QTimer::singleShot(3000, [&] () { app.exit(); });

    app.exec();
  }
#endif

