#include <ossia/detail/config.hpp>

#include <ossia/dataflow/exec_state_facade.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/nodes/faust/faust_node.hpp>

#include "include_catch.hpp"

#include <memory>

namespace
{
//! One audio input, one audio output that plays the value of a button and
//! a slider added together, and a checkbox that does not play.
struct button_dsp final : dsp
{
  FAUSTFLOAT button{};
  FAUSTFLOAT checkbox{};
  FAUSTFLOAT slider{};
  int sample_rate{};

  int getNumInputs() override { return 1; }
  int getNumOutputs() override { return 1; }
  void buildUserInterface(UI* ui) override
  {
    ui->openVerticalBox("root");
    ui->addButton("trigger", &button);
    ui->addCheckButton("toggle", &checkbox);
    ui->addHorizontalSlider("level", &slider, 0., 0., 1., 0.01);
    ui->closeBox();
  }
  int getSampleRate() override { return sample_rate; }
  void init(int sr) override { instanceInit(sr); }
  void instanceInit(int sr) override
  {
    instanceConstants(sr);
    instanceResetUserInterface();
    instanceClear();
  }
  void instanceConstants(int sr) override { sample_rate = sr; }
  void instanceResetUserInterface() override { button = checkbox = slider = 0; }
  void instanceClear() override { }
  dsp* clone() override
  {
    auto d = new button_dsp;
    d->init(sample_rate);
    return d;
  }
  void metadata(Meta*) override { }
  void compute(int count, FAUSTFLOAT** in, FAUSTFLOAT** out) override
  {
    for(int i = 0; i < count; i++)
      out[0][i] = button + slider;
  }
};

template <typename Node>
struct fixture
{
  std::shared_ptr<button_dsp> dsp = [] {
    auto d = std::make_shared<button_dsp>();
    d->init(48000);
    return d;
  }();
  Node node{dsp};
  ossia::execution_state st;
  int64_t date{};

  fixture()
  {
    st.bufferSize = 16;
    auto& in = *node.root_inputs()[0]->template target<ossia::audio_port>();
    in.set_channels(1);
    in.channel(0).assign(st.bufferSize, 0.);
  }

  ossia::value_port& control(int i)
  {
    return *node.root_inputs()[1 + i]->template target<ossia::value_port>();
  }

  //! Runs one tick with `v` on control `i` (none if invalid), returns the
  //! first output sample.
  double tick(int i = 0, ossia::value v = {})
  {
    for(int c = 0; c < 3; c++)
      control(c).clear();
    if(v.valid())
      control(i).write_value(std::move(v), 0);

    ossia::token_request tk{
        ossia::time_value{date},   ossia::time_value{date + st.bufferSize},
        ossia::time_value{100000}, ossia::time_value{0},
        1.,                        ossia::time_signature{4, 4},
        120.};
    date += st.bufferSize;
    static_cast<ossia::graph_node&>(node).run(tk, ossia::exec_state_facade{&st});

    auto& out = *node.root_outputs()[0]->template target<ossia::audio_port>();
    REQUIRE(out.channels() >= 1);
    REQUIRE(out.channel(0).size() >= 1);
    return out.channel(0)[0];
  }
};
}

// A button is held while its value is non-zero; an impulse presses it for one
// tick only.
TEMPLATE_TEST_CASE(
    "faust_button_impulse", "faust", ossia::nodes::faust_fx,
    ossia::nodes::faust_mono_fx)
{
  fixture<TestType> f;
  REQUIRE(f.control(0).is_event);
  REQUIRE_FALSE(f.control(1).is_event);
  REQUIRE_FALSE(f.control(2).is_event);

  SECTION("impulse")
  {
    CHECK(f.tick(0, ossia::impulse{}) == 1.);
    CHECK(f.dsp->button == 0.f);
    CHECK(f.tick() == 0.);
  }

  SECTION("held")
  {
    CHECK(f.tick(0, 1.f) == 1.);
    CHECK(f.tick() == 1.);
    CHECK(f.tick(0, 0.f) == 0.);
    CHECK(f.tick() == 0.);
  }

  SECTION("impulse does not reach the slider")
  {
    CHECK(f.tick(2, 0.5f) == 0.5);
    CHECK(f.tick(0, ossia::impulse{}) == 1.5);
    CHECK(f.tick() == 0.5);
  }

  SECTION("checkbox is not an event")
  {
    f.tick(1, 1.f);
    CHECK(f.dsp->checkbox == 1.f);
    f.tick();
    CHECK(f.dsp->checkbox == 1.f);
  }
}
