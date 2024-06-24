#include "default_policy.hpp"

#include <ossia/dataflow/execution/to_state_element.hpp>
#include <ossia/editor/state/detail/state_flatten_visitor.hpp>

namespace ossia
{

void default_execution_state_policy::commit()
{
  state_flatten_visitor<ossia::flat_vec_state, false, true> vis{m_commitOrderedState};
  for(auto it = m_valueState.begin(), end = m_valueState.end(); it != end; ++it)
  {
    auto& [param, vec] = *it;
    switch(vec.size())
    {
      case 0:
        continue;
      case 1: {
        to_state_element(*param, vec[0].first).launch();
        break;
      }
      default: {
        m_commitOrderedState.clear();
        m_commitOrderedState.reserve(vec.size());
        for(auto& val : vec)
        {
          // std::cerr << "mergin : " <<  val.first.value << std::endl;
          vis(to_state_element(*param, std::move(val.first)));
        }

        m_commitOrderedState.launch();
      }
    }

    vec.clear();
  }

  commit_common();
}

}
