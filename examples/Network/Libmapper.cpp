#include <ossia/network/local/local.hpp>
#include <ossia/network/libmapper/libmapper_protocol.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/common/complex_type.hpp>
int main()
{
  auto source_proto = std::make_unique<ossia::net::libmapper_server_protocol>();
  auto& sp = *source_proto;
  ossia::net::generic_device source_dev{std::move(source_proto), "source"};
  auto p = ossia::create_parameter(source_dev, "/foo/bar", "float");
  p->push_value(1234.f);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  auto target_proto = std::make_unique<ossia::net::libmapper_client_protocol>();
  auto& tgt = *target_proto;
  ossia::net::generic_device target_dev{std::move(target_proto), "target"};


  while(true)
  {
    tgt.poll(10);

    tgt.update(target_dev);
  }
}
