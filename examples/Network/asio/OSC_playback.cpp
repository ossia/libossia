// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/detail/timer.hpp>
#include <ossia/network/base/message_queue.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/detail/typetag.hpp>
#include <ossia/protocols/osc/osc_factory.hpp>

#include <fmt/ranges.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include <chrono>
#include <iostream>

// OSC dump format:
// header: index table:
// - OSC address
// - typetag
// - length
// data:
// [ index ] [ timestamp ] [ f32 ] [ f32 ]

struct recorded_automations
{
  const char* file{};
  int64_t pos = 0;
  int64_t size = 0;

  struct entry
  {
    std::string name;
    std::string tag;
    uint32_t size{};
    ossia::net::parameter_base* parameter{};
  };

  std::vector<entry> entries;

  void read_to_next_boundary()
  {
    // Read at most 4 zeros
    int k = 3;
    while(file[pos] == 0 && pos < size && k-- > 0)
      pos++;
  }
};

namespace
{
static void read_header(recorded_automations& ctx)
{
  auto file = ctx.file;

  if(memcmp(file + ctx.pos, "OSCR", 4) != 0)
    return;

  ctx.pos += 4;
  int32_t count{};
  memcpy(&count, file + ctx.pos, 4);
  ctx.pos += 4;

  for(int i = 0; i < count; i++)
  {
    recorded_automations::entry e;
    // Read the name
    e.name = file + ctx.pos;
    ctx.pos += e.name.length() + 1;
    ctx.read_to_next_boundary();

    // Read the tag
    if(file[ctx.pos] != 0)
    {
      e.tag = file + ctx.pos;
      ctx.pos += e.tag.length() + 1;
    }
    ctx.read_to_next_boundary();

    // Read the length
    memcpy(&e.size, file + ctx.pos, 4);
    ctx.pos += 4;

    ctx.entries.push_back(std::move(e));
  }
}

template <typename T>
struct file_entry
{
  int32_t index{};
  int64_t timestamp{};
  T data;
};

template <typename T>
void process(
    recorded_automations& ret, const recorded_automations::entry& e,
    const file_entry<T>& entry)
{
  e.parameter->push_value(entry.data);
}

void process(recorded_automations& ret, recorded_automations::entry& entry)
{
  if(entry.tag == "f")
  {
    process(ret, entry, *reinterpret_cast<const file_entry<float>*>(ret.file + ret.pos));
  }
  else if(entry.tag == "i")
  {
    process(ret, entry, *reinterpret_cast<const file_entry<int>*>(ret.file + ret.pos));
  }
  else if(entry.tag == "b")
  {
    process(ret, entry, *reinterpret_cast<const file_entry<bool>*>(ret.file + ret.pos));
  }
  else if(entry.tag == "ff")
  {
    process(
        ret, entry,
        *reinterpret_cast<const file_entry<std::array<float, 2>>*>(ret.file + ret.pos));
  }
  else if(entry.tag == "fff")
  {
    process(
        ret, entry,
        *reinterpret_cast<const file_entry<std::array<float, 3>>*>(ret.file + ret.pos));
  }
  else if(entry.tag == "ffff")
  {
    process(
        ret, entry,
        *reinterpret_cast<const file_entry<std::array<float, 4>>*>(ret.file + ret.pos));
  }

  ret.pos += entry.size;
}
}

int main(int argc, char** argv)
{
  // Set-up logger
  ossia::context c;

  if(argc != 3)
  {
    ossia::logger().error(
        "Invalid number of arguments.\n"
        "Invocation: ./OSC_player <ip:port> <input file>\n");
    return 1;
  }

  std::string host = argv[1];
  std::string file = argv[2];
  struct stat st;
  stat(file.c_str(), &st);
  auto filesize = st.st_size;

  int fd = open(file.c_str(), O_RDONLY, 0);
  assert(fd != -1);

  auto mmap_flags = MAP_PRIVATE;
#if defined(__linux__)
  mmap_flags |= MAP_POPULATE;
#endif
  void* data = mmap(NULL, filesize, PROT_READ, mmap_flags, fd, 0);
  assert(data != MAP_FAILED);

  recorded_automations ret;
  ret.file = (const char*)data;
  ret.size = filesize;

  read_header(ret);

  // Create the OSC device
  auto ctx = std::make_shared<ossia::net::network_context>();
  auto protocol = ossia::net::make_osc_protocol(
      ctx, {.transport = ossia::net::udp_configuration{
                {.local = {},
                 .remote = ossia::net::outbound_socket_configuration{
                     .host = "127.0.0.1", .port = 4456}}}});

  ossia::net::generic_device device{std::move(protocol), "B"};

  for(auto& entry : ret.entries)
  {
    auto& n = ossia::net::create_node(device.get_root_node(), entry.name);
    if(entry.tag == "f")
    {
      entry.parameter = n.create_parameter(ossia::val_type::FLOAT);
    }
    else if(entry.tag == "i")
    {
      entry.parameter = n.create_parameter(ossia::val_type::INT);
    }
    else if(entry.tag == "b")
    {
      entry.parameter = n.create_parameter(ossia::val_type::BOOL);
    }
    else if(entry.tag == "ff")
    {
      entry.parameter = n.create_parameter(ossia::val_type::VEC2F);
    }
    else if(entry.tag == "fff")
    {
      entry.parameter = n.create_parameter(ossia::val_type::VEC3F);
    }
    else if(entry.tag == "ffff")
    {
      entry.parameter = n.create_parameter(ossia::val_type::VEC4F);
    }
    else if(entry.tag == "")
    {
      entry.parameter = n.create_parameter(ossia::val_type::IMPULSE);
    }
    else
    {
      entry.parameter = n.create_parameter(ossia::val_type::LIST);
    }
  }

  // Read and send our stuff
  while(ret.pos < ret.size)
  {
    int32_t index{};
    memcpy(&index, ret.file + ret.pos, sizeof(index));

    process(ret, ret.entries[index]);
  }

  return 0;
}
