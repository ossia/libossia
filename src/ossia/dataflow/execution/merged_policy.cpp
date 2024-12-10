#include "merged_policy.hpp"

#include <ossia/dataflow/execution/to_state_element.hpp>
#include <ossia/editor/state/detail/state_flatten_visitor.hpp>

namespace ossia
{

void merged_execution_state_policy::commit()
{
  // int i = 0;
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
        m_monoState.e = ossia::state_element{};
        state_flatten_visitor<ossia::mono_state, false, true> vis{m_monoState};
        // i += vec.size();
        for(auto& val : vec)
        {
          vis(to_state_element(*param, std::move(val.first)));
        }
        ossia::launch(m_monoState.e);
        break;
      }
    }
    vec.clear();
  }
  // std::cout << "NUM MESSAGES: " << i << std::endl;

  commit_common();
}

#if !defined(OSSIA_FREESTANDING)
threaded_merged_execution_state_policy::threaded_merged_execution_state_policy()
{
  m_valuesOutputThread = std::thread{[this] {
    while(!m_stopFlag)
    {
      std::vector<ossia::state_element> m;
      while(m_messagesToApply.wait_dequeue_timed(m, 20'000))
      {
        // !!! FIXME make sure that this does not contain the Control Messages
        for(auto& mm : m)
          ossia::launch(mm);
      }
    }
  }};
}

threaded_merged_execution_state_policy::~threaded_merged_execution_state_policy()
{
  m_stopFlag = true;
  m_valuesOutputThread.join();
}

inline void to_state_element(
    ossia::net::parameter_base& p, const ossia::typed_value& v,
    std::vector<state_element>& out)
{
  auto& res
      = out.emplace_back(boost::variant2::in_place_type_t<ossia::message>{}, p, v.value);

  auto& m = *res.target<ossia::message>();
  if(auto u = v.type.target<ossia::unit_t>())
    m.dest.unit = std::move(*u);
  m.dest.index = std::move(v.index);
}

void threaded_merged_execution_state_policy::commit()
{
  // int i = 0;
  m_states.clear();
  m_states.reserve(10000);
  for(auto it = m_valueState.begin(), end = m_valueState.end(); it != end; ++it)
  {
    auto& [param, vec] = *it;
    switch(vec.size())
    {
      case 0:
        continue;
      case 1: {
        to_state_element(*param, vec[0].first, m_states);
        //to_state_element(*param, vec[0].first).launch();
        break;
      }
      default: {
        m_monoState.e = ossia::state_element{};
        state_flatten_visitor<ossia::mono_state, false, true> vis{m_monoState};
        // i += vec.size();
        for(auto& val : vec)
        {
          vis(to_state_element(*param, std::move(val.first)));
        }

        m_states.push_back(std::move(m_monoState.e));
        //ossia::launch(m_monoState.e);
        break;
      }
    }
    vec.clear();
  }
  this->m_messagesToApply.enqueue(std::move(m_states));
  // std::cout << "NUM MESSAGES: " << i << std::endl;

  commit_common();
}
#endif
}
