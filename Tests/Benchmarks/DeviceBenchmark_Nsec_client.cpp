// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <iostream>
#include "Random.hpp"
#include<thread>
#include <boost/range/algorithm/find_if.hpp>
#if __has_include(<valgrind/callgrind.h>)
#include <valgrind/callgrind.h>
#endif
Random r;
const ossia::net::node_base* goToRandomNode(
    const ossia::net::node_base* root)
{
  // Get a random number between 1 and 100
  auto depth = 1 + r.getRandomUInt() % 100;

  // Try to go randomly to a node that deep and return it
  auto currentNode = root;
  for(int i = 0; i < depth; i++)
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


void add_children(ossia::net::node_base& root,
                  std::vector<ossia::net::parameter_base*>& nodes)
{
  for(auto& child : root.children())
  {
    if(auto p = child->get_parameter())
      nodes.push_back(p);
    add_children(*child, nodes);
  }
}

int main(int argc, char** argv)
{
  auto proto = new ossia::oscquery::oscquery_mirror_protocol("ws://127.0.0.1:5678");
  ossia::net::generic_device remote{std::unique_ptr<ossia::oscquery::oscquery_mirror_protocol>{proto}, "B"};
  if(proto->update_future(remote.get_root_node()).wait_for(std::chrono::seconds(10)) != std::future_status::ready)
    return 1;
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
      add_children(*node, other_addr);

    if(start_addr && stop_addr)
      break;
  }
  assert(start_addr);
  assert(stop_addr);

  std::atomic_bool a = false;
  std::atomic_bool b = false;
  std::chrono::steady_clock::duration dur;
  std::thread checktime([&] () {
    a = true;
    auto t0 = std::chrono::steady_clock::now();
    while(true)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      dur = std::chrono::steady_clock::now() - t0;
      if(dur > std::chrono::milliseconds(100))
      {
        b = true;
        break;
      }
    }
  });

  while(!a);

  int num_sent = 0;
  start_addr->push_value(ossia::impulse{});
#if __has_include(<valgrind/callgrind.h>)
  CALLGRIND_START_INSTRUMENTATION;
#endif
  while(!b)
  {
    // Select a new node
    auto p = goToRandomNode(other_addr);
    p->push_value(0.f);

    num_sent ++;
  }
#if __has_include(<valgrind/callgrind.h>)
  CALLGRIND_STOP_INSTRUMENTATION;
  CALLGRIND_DUMP_STATS;
#endif
  std::cout << "Sent " << num_sent
            << " values in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(dur).count()
            <<  " milliseconds" << std::endl;
  stop_addr->push_value(ossia::impulse{});

  checktime.join();

  std::this_thread::sleep_for(std::chrono::seconds(3));
}
