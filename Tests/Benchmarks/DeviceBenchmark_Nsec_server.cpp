// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>
#include "Random.hpp"
#include <thread>
#include <atomic>
#include <boost/range/algorithm/find_if.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
Random r;

ossia::net::node_base* goToRandomNode(
    ossia::net::node_base& root)
{
  // Get a random number between 1 and 100
  auto depth = 1 + r.getRandomUInt() % 100;

  // Try to go randomly to a node that deep and return it
  auto currentNode = &root;
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

int main(int argc, char** argv)
{
  std::size_t num_nodes = 1000;
  if(argc > 1)
  {
    auto num = std::atoi(argv[1]);
    if(num > 0 && num < std::numeric_limits<int32_t>::max())
      num_nodes = num;
  }
  std::atomic_int num_received{0};
  std::atomic_int received_on_stop{0};
  ossia::net::generic_device local{
    std::make_unique<ossia::oscquery::oscquery_server_protocol>(),
        "A"};
  auto localDevice = &local.get_root_node();

  for(std::size_t i = 0; i < num_nodes; i++)
  {
    auto& node = ossia::net::create_node(*goToRandomNode(local.get_root_node()), r.getRandomAddress());

    auto addr = node.create_parameter(ossia::val_type::FLOAT);
    addr->set_critical(true);
    addr->add_callback([&] (const auto& val) {
      ++num_received;
    });
  }

  // Add two nodes for start and stop of the timer.

  std::chrono::steady_clock::time_point start_time;
  std::chrono::steady_clock::time_point stop_time;

  std::atomic_bool b = false;

  auto st = localDevice->create_child("startTick");
  auto st_addr = st->create_parameter(ossia::val_type::IMPULSE);
  st_addr->set_critical(true);
  st_addr->add_callback([&] (const ossia::value& val) {
    start_time = std::chrono::steady_clock::now();
    std::cerr << "START received" << std::endl;
  });
  auto et = localDevice->create_child("stopTick");
  auto et_addr = et->create_parameter(ossia::val_type::IMPULSE);
  et_addr->set_critical(true);
  et_addr->add_callback([&] (const ossia::value& val) {
    stop_time = std::chrono::steady_clock::now();
    received_on_stop.store(num_received);
    b = true;
    std::cerr << "STOP received" << std::endl;
  });

  qDebug() << "running with" << num_nodes;
  while(!b)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }


  std::cout << "Received " << received_on_stop
            << " values in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count()
            << " milliseconds" << std::endl;
}


