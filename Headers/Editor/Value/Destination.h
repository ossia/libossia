#pragma once
#include <ossia_export.h>
#include <vector>
#include <memory>
#include <initializer_list>
#include <Misc/DestinationIndex.h>
#include <Network/AddressProperties.h>
namespace OSSIA
{
class Value;
class Node;
/*! \details Destination to an Address value and optionnally to several index of this value */
struct OSSIA_EXPORT Destination final
{
  std::shared_ptr<Node> value;
  DestinationIndex index;

  Destination();
  Destination(std::shared_ptr<Node> v);
  Destination(std::shared_ptr<Node> v, DestinationIndex);

  Destination(const Destination& other);
  Destination(Destination&& other);
  Destination& operator=(const Destination&);
  Destination& operator=(Destination&&);

  bool operator== (const Value&) const;
  bool operator!= (const Value&) const;
  bool operator>  (const Value&) const;
  bool operator>= (const Value&) const;
  bool operator<  (const Value&) const;
  bool operator<= (const Value&) const;
};

}
