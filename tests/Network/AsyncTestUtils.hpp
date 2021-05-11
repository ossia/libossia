#pragma once
#include "ProtocolTestUtils.hpp"

#include <ossia/network/context.hpp>

using namespace ossia;

void push_all_values_async(
        int N,
        std::vector<std::vector<ossia::net::parameter_base*>>& local_addr,
        std::vector<std::vector<ossia::net::parameter_base*>> & remote_addr,
    ossia::net::network_context& ctx)
{
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
    ctx.context.poll();
  }

  for(int i = 0; i < N; i++)
  {
    int j = 0;
    for(const ossia::value& val : value_to_test)
    {
      REQUIRE(remote_addr[i][j]->value() == val);
      remote_addr[i][j]->push_value(val);
      j++;
    }
    ctx.context.poll();
  }

  int j = 0;
  for(int i = 0; i < N; i++)
  {
    local_addr[i][j]->set_value_type((ossia::val_type) i);
    local_addr[i][j]->push_value(ossia::init_value((ossia::val_type) i));
    j++;
    ctx.context.poll();
  }

  for(int i = 0; i < N; i++)
  {
    int j = 0;
    for(const ossia::value& val : value_to_test)
    {
      remote_addr[i][j]->push_value(ossia::init_value((ossia::val_type) val.get_type()));
      j++;
    }
    ctx.context.poll();
  }
}

void push_all_values_domain_async(
        int N,
        std::vector<std::vector<ossia::net::parameter_base*>>& local_addr,
        std::vector<std::vector<ossia::net::parameter_base*>> & remote_addr,
    ossia::net::network_context& ctx)
{
  for(int i = 0; i < N; i++)
  {
    int j = 0;
    for(const ossia::value& val : value_to_test)
    {
      local_addr[i][j]->set_value_type(val.get_type());
      local_addr[i][j]->push_value(val);
      j++;
    }
    ctx.context.poll();
  }

  for(int i = 0; i < N; i++)
  {
    int j = 0;
    for(const ossia::value& val : value_to_test)
    {
      remote_addr[i][j]->push_value(val);
      j++;
    }
    ctx.context.poll();
  }

  int j = 0;
  for(int i = 0; i < N; i++)
  {
    local_addr[i][j]->set_value_type((ossia::val_type) i);
    local_addr[i][j]->push_value(ossia::init_value((ossia::val_type) i));
    j++;
    ctx.context.poll();
  }

  for(int i = 0; i < N; i++)
  {
    int j = 0;
    for(const ossia::value& val : value_to_test)
    {
      remote_addr[i][j]->push_value(ossia::init_value((ossia::val_type) val.get_type()));
      j++;
    }
    ctx.context.poll();
  }
}



template<typename FunProto1, typename FunProto2>
void test_comm_generic_async(
    FunProto1 local_proto,
    FunProto2 remote_proto,
    ossia::net::network_context& ctx)
{
  int N = 10;
  remote_data rem{std::move(local_proto), std::move(remote_proto)};

  // Connect is async, it runs there
  ctx.context.poll_one();

  auto& local_addr = rem.local_addr;
  auto& remote_addr = rem.remote_addr;

  push_all_values_async(N, local_addr, remote_addr, ctx);

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

  push_all_values_async(N, local_addr, remote_addr, ctx);


  auto test_all_values = [&] (std::vector<ossia::net::parameter_base*>& vec)
  {
    for(int access_i = 0; access_i < 3; access_i++)
    {
      for(int i = 0; i < N; i++)
      {
        vec[i]->set_access(access_mode(access_i));
      }

      push_all_values_domain_async(N, local_addr, remote_addr, ctx);

      for(int bounding_i = 0 ; bounding_i < 6; bounding_i++)
      {
        for(int i = 0; i < N; i++)
        {
          vec[i]->set_bounding(bounding_mode(bounding_i));
        }

        push_all_values_domain_async(N, local_addr, remote_addr, ctx);

        for(int domain_i = 0; domain_i < N; domain_i++)
        {
          vec[domain_i]->set_domain(init_domain(ossia::val_type(domain_i)));
        }

        push_all_values_domain_async(N, local_addr, remote_addr, ctx);


        for(int domain_i = 0; domain_i < N; domain_i++)
        {
          auto val = ossia::init_value((ossia::val_type) domain_i);
          auto dom = ossia::make_domain(val, val);
          vec[domain_i]->set_domain(dom);
        }

        push_all_values_domain_async(N, local_addr, remote_addr, ctx);

      }
    }
  };

  int k = 0;
  for(auto& vec : local_addr)
  {
    test_all_values(vec);
  }


  k = 0;
  for(auto& vec : remote_addr)
  {
    test_all_values(vec);
  }
}
