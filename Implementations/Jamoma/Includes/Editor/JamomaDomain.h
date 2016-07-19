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
#include <Editor/Value/Value.h>

using namespace OSSIA;
using namespace std;

class JamomaDomain final : public Domain
{

private:

# pragma mark -
# pragma mark Implementation specific

  Value * mMin;
  Value * mMax;
  vector<const Value*> mValues;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaDomain(const Value * = new Impulse(),
               const Value * = new Impulse(),
               vector<const Value*> = vector<const Value*>());

  JamomaDomain(const JamomaDomain *);

  shared_ptr<Domain> clone() const override;

  ~JamomaDomain();

# pragma mark -
# pragma mark Accessors

  const Value * getMin() const override;

  Domain & setMin(const Value*) override;

  const Value * getMax() const override;

  Domain & setMax(const Value*) override;

  vector<const Value*> getValues() const override;

  Domain & setValues(vector<const Value*> values) override;

};

