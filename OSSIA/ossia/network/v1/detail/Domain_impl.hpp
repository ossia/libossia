/*!
 * \file Domain_impl.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include "Editor/Domain.hpp"
#include <Editor/Value/Value.h>

using namespace OSSIA;
using namespace std;

namespace impl
{
class JamomaDomain final : public Domain
{

private:

# pragma mark -
# pragma mark Implementation specific

  Value mMin;
  Value mMax;
  vector<Value> mValues;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaDomain(const Value&,
               const Value&,
               std::vector<Value>);

  JamomaDomain(const JamomaDomain&);

  shared_ptr<Domain> clone() const override;

  ~JamomaDomain();

# pragma mark -
# pragma mark Accessors

  const Value& getMin() const override;

  Domain & setMin(const Value&) override;

  const Value& getMax() const override;

  Domain & setMax(const Value&) override;

  const std::vector<Value>& getValues() const override;

  Domain & setValues(const std::vector<Value>& values) override;

};
}
