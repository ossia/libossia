#pragma once
#include <ossia/network/common/parameter_properties.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include <thread>
#include "include_catch.hpp"
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
{ using type = ossia::domain_base<int32_t>; };
template<>
struct matching_domain<ossia::val_type::FLOAT>
{ using type = ossia::domain_base<float>; };
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

static constexpr int NUM_ADDRESSES_TO_TEST = ((int) ossia::val_type::MAP);
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
    std::string{"Welcome to the wonderful ossia world!"},
    ossia::value(std::vector<ossia::value>{}),
    std::vector<ossia::value>{ossia::impulse{}},
    std::vector<ossia::value>{ossia::impulse{}, ossia::impulse{}},
    std::vector<ossia::value>{int32_t{0}},
    std::vector<ossia::value>{int32_t{0}, int32_t{1}},
    std::vector<ossia::value>{float{0}, int32_t{1}},
    std::vector<ossia::value>{float{0}, int32_t{1}, ossia::impulse{}, std::string{}},
    std::vector<ossia::value>{float{0}, int32_t{1}, ossia::impulse{}, std::string{"foo bar"}},
    std::vector<ossia::value>{float{0}, float{1000}},
    ossia::vec2f{},
    ossia::vec3f{},
    ossia::vec4f{}
};



struct remote_data
{
  template<typename FunProto1, typename FunProto2>
  remote_data(
      FunProto1 local_proto,
      FunProto2 remote_proto):
    local_device{local_proto(), "score" },
    remote_device{remote_proto(), "score-remote"}
  {
    int N = NUM_ADDRESSES_TO_TEST;

    for(int i = 0; i < N; i++)
    {
      auto cld = local_device.create_child(std::to_string(i));
      std::vector<ossia::net::parameter_base*> params;
      params.reserve(value_to_test.size());
      for(size_t j = 0; j<value_to_test.size(); j++)
      {
        auto node = cld->create_child(std::to_string(j));
        auto param = node->create_parameter(value_to_test[j].get_type());
        params.push_back(param);
      }
      local_addr.push_back(params);
    }

    for(int i = 0; i < N; i++)
    {
      auto cld = remote_device.create_child(std::to_string(i));
      std::vector<ossia::net::parameter_base*> params;
      params.reserve(value_to_test.size());
      for(size_t j = 0; j<value_to_test.size(); j++)
      {
        auto node = cld->create_child(std::to_string(j));
        auto param = node->create_parameter(value_to_test[j].get_type());
        params.push_back(param);
      }
      remote_addr.push_back(params);
    }
  }

  ossia::net::generic_device local_device;
  ossia::net::generic_device remote_device;

  std::vector<std::vector<ossia::net::parameter_base*>> local_addr;
  std::vector<std::vector<ossia::net::parameter_base*>> remote_addr;
};


void push_all_values(
        std::vector<std::vector<ossia::net::parameter_base*>>& local_addr,
        std::vector<std::vector<ossia::net::parameter_base*>> & remote_addr)
{
  int N = NUM_ADDRESSES_TO_TEST;
  for(int i = 0; i < N; i++)
  {
    int j = 0;
    for(const ossia::value& val : value_to_test)
    {
      local_addr[i][j]->set_value_type(val.get_type());
      local_addr[i][j]->push_value(val);
      REQUIRE(local_addr[i][j]->value() == val);
      j++;
    }
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  for(int i = 0; i < N; i++)
  {
    int j = 0;
    for(const ossia::value& val : value_to_test)
    {
      REQUIRE(remote_addr[i][j]->value() == val);
      remote_addr[i][j]->push_value(val);
      j++;
    }
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  int j = 0;
  for(int i = 0; i < N; i++)
  {
    local_addr[i][j]->set_value_type((ossia::val_type) i);
    local_addr[i][j]->push_value(ossia::init_value((ossia::val_type) i));
    j++;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  for(int i = 0; i < N; i++)
  {
    int j = 0;
    for(const ossia::value& val : value_to_test)
    {
      remote_addr[i][j]->push_value(ossia::init_value((ossia::val_type) val.get_type()));
      j++;
    }
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
}



// In this case we don't do checking yet as there are a lot of possible cases -
// the parameters are set with their default domains
void push_all_values_domain(
        std::vector<std::vector<ossia::net::parameter_base*>>& local_addr,
        std::vector<std::vector<ossia::net::parameter_base*>> & remote_addr)
{
  int N = NUM_ADDRESSES_TO_TEST;
  for(int i = 0; i < N; i++)
  {
    int j = 0;
    for(const ossia::value& val : value_to_test)
    {
      local_addr[i][j]->set_value_type(val.get_type());
      local_addr[i][j]->push_value(val);
      j++;
    }
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  for(int i = 0; i < N; i++)
  {
    int j = 0;
    for(const ossia::value& val : value_to_test)
    {
      remote_addr[i][j]->push_value(val);
      j++;
    }
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  int j = 0;
  for(int i = 0; i < N; i++)
  {
    local_addr[i][j]->set_value_type((ossia::val_type) i);
    local_addr[i][j]->push_value(ossia::init_value((ossia::val_type) i));
    j++;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  for(int i = 0; i < N; i++)
  {
    int j = 0;
    for(const ossia::value& val : value_to_test)
    {
      remote_addr[i][j]->push_value(ossia::init_value((ossia::val_type) val.get_type()));
      j++;
    }
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
}


template<typename FunProto1, typename FunProto2>
void test_comm_generic(
    FunProto1 local_proto,
    FunProto2 remote_proto)
{
  int N = NUM_ADDRESSES_TO_TEST;
  remote_data rem{std::move(local_proto), std::move(remote_proto)};
  auto& local_addr = rem.local_addr;
  auto& remote_addr = rem.remote_addr;


  push_all_values(local_addr, remote_addr);


  for(const auto& vec : local_addr)
  {
    for(auto val : vec)
    {
      rem.local_device.get_protocol().observe(*val, true);
    }
  }
  for(const auto& vec : remote_addr)
  {
    for(auto val : vec)
    {
      rem.remote_device.get_protocol().observe(*val, true);
    }
  }

  push_all_values(local_addr, remote_addr);


  auto test_all_values = [&] (std::vector<ossia::net::parameter_base*>& vec)
  {
    for(int access_i = 0; access_i < 3; access_i++)
    {
      for(int i = 0; i < N; i++)
      {
        vec[i]->set_access(access_mode(access_i));
      }

      push_all_values_domain(local_addr, remote_addr);
      for(int bounding_i = 0 ; bounding_i < 6; bounding_i++)
      {
        for(int i = 0; i < N; i++)
        {
          vec[i]->set_bounding(bounding_mode(bounding_i));
        }

        push_all_values_domain(local_addr, remote_addr);

        for(int domain_i = 0; domain_i < N; domain_i++)
        {
          vec[domain_i]->set_domain(init_domain(ossia::val_type(domain_i)));
        }

        push_all_values_domain(local_addr, remote_addr);


        for(int domain_i = 0; domain_i < N; domain_i++)
        {
          auto val = ossia::init_value((ossia::val_type) domain_i);
          auto dom = ossia::make_domain(val, val);
          vec[domain_i]->set_domain(dom);
        }

        push_all_values_domain(local_addr, remote_addr);

      }
    }
  };

  for(auto& vec : local_addr)
    test_all_values(vec);

  for(int i = 0; i < 10; i++)
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

  for(auto& vec : remote_addr)
    test_all_values(vec);

  for(int i = 0; i < 10; i++)
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

}



