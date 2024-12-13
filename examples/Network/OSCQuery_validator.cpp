#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/detail/json_parser.hpp>
#include <ossia/network/oscquery/detail/json_reader_detail.hpp>
#include <ossia/network/oscquery/detail/json_query_parser.hpp>
#include <rapidjson/istreamwrapper.h>
#include <magic_enum/magic_enum.hpp>

int main(int argc, char** argv)
{
  if(argc < 1)
    return -1;

  std::ifstream input{argv[1], std::ios_base::in | std::ios_base::binary};

  auto document = std::make_shared<rapidjson::Document>();

  rapidjson::IStreamWrapper isw(input);
  document->ParseStream(isw);
  if(document->HasParseError())
    perror("Invalid json");


  ossia::oscquery::json_parser p;
  ossia::net::generic_device d("foo");
  p.parse_namespace(d.get_root_node(), *document);

  ossia::net::iterate_all_children(&d.get_root_node(), [] (ossia::net::parameter_base& parameter) {
    std::cerr << parameter.get_node().osc_address()<< ": \n";
    std::cerr << " - type:" << magic_enum::enum_name(parameter.get_value_type()) << "\n";
    std::cerr << " - value:" << ossia::value_to_pretty_string(parameter.value()) << "\n";
    std::cerr << " - min:" << ossia::convert<std::string>(parameter.get_domain().get_min()) << "\n";
    std::cerr << " - max:" << ossia::convert<std::string>(parameter.get_domain().get_max()) << "\n\n";
  });
  return 0;
}
