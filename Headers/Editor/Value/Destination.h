#pragma once
#include <Editor/Value/ValueBase.h>
#include <Network/Node.h>

#include <initializer_list>
namespace OSSIA
{

/*! \details Destination to an Address value and optionnally to several index of this value */
struct OSSIA_EXPORT Destination final : public Value
{
  std::shared_ptr<Node> value;
  std::vector<char> index;

  Destination():
      Value{Type::DESTINATION}
  {

  }

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
  Destination(const Destination& other) = delete;
  virtual ~Destination();

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

}
