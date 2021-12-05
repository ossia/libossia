// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <ossia/detail/timer.hpp>
#include <ossia/protocols/oscquery/oscquery_mirror_asio.hpp>
#include <ossia/network/context.hpp>
#include <ossia/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/base/message_queue.hpp>
#include <ossia/network/oscquery/detail/typetag.hpp>
#include <chrono>


// OSC dump format:
// header: index table:
// - OSC address
// - typetag
// - length
// data:
// [ index ] [ timestamp ] [ f32 ] [ f32 ]

struct recorder {
  FILE* file{};
  int64_t pos{};

  std::unordered_map<ossia::net::node_base*, int32_t> indices;
};

namespace
{
static
void write_header(recorder& ctx, const std::vector<ossia::net::node_base*>& nodes)
{
  auto file = ctx.file;

  fwrite("OSCR", 1, 4, file);

  ctx.pos += 4;
  for(auto* node : nodes)
  {
    auto p = node->get_parameter();
    if(!p)
      continue;

    // Write the address
    auto& addr = node->osc_address();
    ctx.pos += fwrite(addr.c_str(), 1, addr.length() + 1, file);
    while(ctx.pos % 4 != 0)
    {
      ctx.pos += fwrite("", 1, 1, file);
    }

    // Write the typetag
    const auto tt = ossia::oscquery::get_osc_typetag(*node);
    {
      const std::string& tag = *tt;
      // We only handle this for now
      if(tag != "f")
        continue;

      ctx.pos += fwrite(tag.c_str(), 1, tag.length() + 1, file);
      while(ctx.pos % 4 != 0)
      {
        ctx.pos += fwrite("", 1, 1, file);
      }
    }

    // Write the size of messages with that index for easy lookup
    static constexpr int index_size = sizeof(int32_t);
    static constexpr int timestamp_size = sizeof(int64_t);
    uint32_t osc_message_sz = index_size + timestamp_size;
    osc_message_sz += sizeof(float);
    ctx.pos += fwrite(&osc_message_sz, sizeof(osc_message_sz), 1, file);

    ctx.pos += fwrite("", 1, 1, file);
    while(ctx.pos % 4 != 0)
    {
      ctx.pos += fwrite("", 1, 1, file);
    }
  }
}

static int64_t received_messages{};
static
auto write_message(recorder& ctx, ossia::net::node_base* node, const ossia::value& v)
{
  using clk = std::chrono::high_resolution_clock;

  auto t = clk::now().time_since_epoch().count();
  if(auto it = ctx.indices.find(node); it != ctx.indices.end())
  {
    if(const float* res = v.target<float>())
    {
      ctx.pos += fwrite(&it->second, sizeof(it->second), 1, ctx.file);
      ctx.pos += fwrite(&t, sizeof(t), 1, ctx.file);
      ctx.pos += fwrite(res, sizeof(*res), 1, ctx.file);

      received_messages++;
    }
  }
}
}

int main(int argc, char** argv)
{
  // Set-up logger
  ossia::context c;

  if(argc != 4)
  {
    ossia::logger().error("Invalid number of arguments.\n"
                          "Invocation: ./OSCQuery_recorder <ip:port> <osc address pattern> <output file>\n");
    return 1;
  }

  std::string host = argv[1];
  std::string pattern = argv[2];
  std::string file = argv[3];

  auto ctx = std::make_shared<ossia::net::network_context>();
  auto protocol = new ossia::oscquery_asio::oscquery_mirror_asio_protocol{ctx, "ws://127.0.0.1:5678"};

  ossia::net::generic_device device{std::unique_ptr<ossia::net::protocol_base>(protocol), "B"};
  device.get_protocol().update(device);

  auto nodes = ossia::net::find_nodes(device.get_root_node(), pattern);


  auto f = fopen(file.c_str(), "w");
  recorder rec{.file = f};

  for(std::size_t i = 0; i < nodes.size(); i++)
    rec.indices[nodes[i]] = i;

  write_header(rec, nodes);
  for(auto& node : nodes)
  {
    ossia::logger().warn("Found node: {}", node->osc_address());
    node->get_parameter()->add_callback([&, node] (const ossia::value& v) {
      write_message(rec, node, v);
    });
  }

  using namespace std::chrono_literals;
  ossia::timer print_timer{ctx->context};
  print_timer.set_delay(1000ms);
  print_timer.start([&] {
    ossia::logger().info("Received: {} messages", received_messages);
  });

  ctx->context.run_for(10s);

  fclose(f);
  return 0;
}
