#include <ossia/ossia.hpp>
int main(int argc, char** argv)
{
  auto local_proto_ptr = std::make_unique<ossia::net::local_protocol>();
  ossia::net::local_protocol& local_proto = *local_proto_ptr;
  // declare this program "B" as Local device
  ossia::net::generic_device device{std::move(local_proto_ptr), "newDevice"};

  auto localTestNode = device.createChild("test");

  for(int k : {10, 100})
  {
      auto n1 = localTestNode->createChild(std::to_string(k));
      for(int i = 0; i < k; i++)
      {
          auto n2 = n1->createChild(std::to_string(k));
          auto addr = n2->createAddress(ossia::val_type::INT);
          addr->pushValue(i);
      }
  }

  auto n1 = localTestNode->createChild(std::to_string(1000));

  for(int k = 1; k <= 9; k++)
  {
      auto node = localTestNode->createChild(std::string(k, k + '0'));
      auto addr = node->createAddress(ossia::val_type::INT);
      addr->pushValue(k);
  }
  // declare a distant program as a Minuit device
  local_proto.exposeTo(std::make_unique<ossia::net::minuit_protocol>("newDevice", "127.0.0.1", 13579, 9998));

  while (true)
    ;
}
