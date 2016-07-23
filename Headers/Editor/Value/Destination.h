#pragma once
#include <Network/Node.h>
#include <ossia_export.h>

#include <initializer_list>
namespace OSSIA
{
class Value;
/*! \details Destination to an Address value and optionnally to several index of this value */
struct OSSIA_EXPORT Destination final
{
  std::shared_ptr<Node> value;
  std::vector<char> index;

  Destination() = default;
  /*! constructor for a node and optionnal index values
   \param std::shared_ptr<Node>
   \param char
   \param char
   \param ... */

  Destination(std::shared_ptr<Node> v, std::initializer_list<char> = {});

  /*! constructor for a node and an index vector
   \param std::shared_ptr<Node>
   \param std::vector<const #Value> value */
  Destination(std::shared_ptr<Node> v, std::vector<char>);
  Destination(const Destination& other) = default;
  Destination(Destination&& other) = default;
  Destination& operator=(const Destination&) = default;
  Destination& operator=(Destination&&) = default;

  virtual ~Destination();

  /*! equal operator */
  bool operator== (const Value&) const;

  /*! different operator */
  bool operator!= (const Value&) const;

  /*! greater than operator */
  bool operator> (const Value&) const;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const;

  /*! less than operator */
  bool operator< (const Value&) const;

  /*! less than and equal operator */
  bool operator<= (const Value&) const;
};

}
