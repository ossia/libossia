#pragma once
#include <ossia/network/base/protocol.hpp>
#include <eggs/variant.hpp>
#include <readerwriterqueue.h>

namespace ossia
{
namespace net
{
class oscquery_namespace_command
{
};
class oscquery_path_added_command
{
};
class oscquery_path_changed_command
{
};

using oscquery_command =
  eggs::variant<
    oscquery_namespace_command,
    oscquery_path_added_command,
    oscquery_path_changed_command>;

class oscquery_protocol
{
public:
private:
  moodycamel::ReaderWriterQueue<oscquery_command> m_commandQueue;
};
}
}
