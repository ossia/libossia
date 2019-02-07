// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include "../catch/catch.hpp"
#include <ossia/detail/config.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <iostream>
#include "Random.hpp"
#include <thread>
#include <cmath>

static Random r;
using namespace ossia;

const ossia::net::node_base* goToRandomNode(
    const ossia::net::node_base* root)
{
  // Get a random number between 1 and 100
  auto depth = 1 + r.getRandomUInt() % 100;

  // Try to go randomly to a node that deep and return it
  auto currentNode = root;
  for(std::size_t i = 0; i < depth; i++)
  {
    if(currentNode->children().empty())
    {
      break;
    }

    auto node_num = r.getRandomUInt() % currentNode->children().size();
    currentNode = currentNode->children()[node_num].get();

  }

  return currentNode;
}

auto goToRandomNode(std::vector<ossia::net::parameter_base*>& p)
{
  return p[r.getRandomUInt() % (p.size() - 1)];
}

TEST_CASE ("device_benchmark_client", "device_benchmark_client")
{
  ossia::net::generic_device remote{std::make_unique<ossia::oscquery::oscquery_mirror_protocol>("ws://127.0.0.1:5678"), "B"};
  remote.get_protocol().update(remote.get_root_node());
  ossia::net::parameter_base* start_addr{};
  ossia::net::parameter_base* stop_addr{};
  std::vector<ossia::net::parameter_base*> other_addr;
  for(const auto& node : remote.children())
  {
    if(node->get_name() == "startTick")
      start_addr = node->get_parameter();
    else if(node->get_name() == "stopTick")
      stop_addr = node->get_parameter();
    else if(auto p = node->get_parameter())
      other_addr.push_back(p);

    if(start_addr && stop_addr)
      break;
  }
  REQUIRE(start_addr);

  start_addr->push_value(ossia::impulse{});
  // Send the "start" tick
  // Send the messages
  int iter = 1000000;
  std::chrono::steady_clock::duration total_dur;
  for(int i = 0; i < iter; i++)
  {
    // Select a new node
    auto p = goToRandomNode(other_addr);
    auto start = std::chrono::steady_clock::now();
    p->push_value(0.f);
    auto end = std::chrono::steady_clock::now();

    total_dur += end - start;
  }

  stop_addr->push_value(ossia::impulse{});
  std::cout << "Sending: " << iter << ": "
            << std::chrono::duration <double, std::milli> (total_dur).count() / float(iter)
      << "" << std::endl;
}

