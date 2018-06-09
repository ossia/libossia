#include <ossia/detail/config.hpp>
#include <QtTest>
#include <ossia/dataflow/graph/tick_methods.hpp>

class TickMethodTest : public QObject
{
    Q_OBJECT
  private Q_SLOTS:
    void test_cuts()
    {
      using namespace ossia;

      ossia::flat_set<int64_t> cuts{3, 7, 9, 35};
      ossia::token_request_vec tokens;
      tokens.push_back(ossia::token_request{0_tv, 5_tv, 0.1});
      tokens.push_back(ossia::token_request{5_tv, 10_tv, 0.2});
      tokens.push_back(ossia::token_request{10_tv, 15_tv, 0.3});
      split_score_tick<&ossia::execution_state::commit>::do_cuts(cuts, tokens, 0_tv);

      // from 0 to 3
      // from 3 to 5
      // from 5 to 7
      // from 7 to 9
      // from 9 to 10
      // from 10


    }
};
QTEST_APPLESS_MAIN(TickMethodTest)

#include "TickMethodTest.moc"
