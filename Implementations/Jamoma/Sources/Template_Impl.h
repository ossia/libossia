/*!
 * \file Template_Impl.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef TEMPLATE_IMPL_H_
#define TEMPLATE_IMPL_H_

#include "Editor/Template.h"

#include "TTScore.h"

namespace OSSIA
{
  class Template_Impl : public Template
  {
  public:
    
    // Constructors, destructor, assignment
    Template_Impl();
    virtual ~Template_Impl();
    
    // Iterators
    virtual const_iterator begin() const override;
    virtual const_iterator end() const override;
    virtual const_iterator find(const TemplateElement&) const override;
    
    /** Implementation Specific
     @details use mutable members to break constness of the API because Jamoma doesn't take care of it.
     */
  };
}

#endif /* TEMPLATE_IMPL_H_ */
