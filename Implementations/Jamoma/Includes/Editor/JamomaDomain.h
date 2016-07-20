/*!
 * \file JamomaDomain.h
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

#include "Editor/Domain.h"
#include <Editor/Value/SafeValue.h>

using namespace OSSIA;
using namespace std;

class JamomaDomain final : public Domain
{

private:

# pragma mark -
# pragma mark Implementation specific

  SafeValue mMin;
  SafeValue mMax;
  vector<SafeValue> mValues;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaDomain(const SafeValue&,
               const SafeValue&,
               std::vector<SafeValue>);

  JamomaDomain(const JamomaDomain&);

  shared_ptr<Domain> clone() const override;

  ~JamomaDomain();

# pragma mark -
# pragma mark Accessors

  const SafeValue& getMin() const override;

  Domain & setMin(const SafeValue&) override;

  const SafeValue& getMax() const override;

  Domain & setMax(const SafeValue&) override;

  const std::vector<SafeValue>& getValues() const override;

  Domain & setValues(const std::vector<SafeValue>& values) override;

};

