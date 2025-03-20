#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/detail/logger.hpp>

#include <faust/gui/UI.h>
#include <libremidi/message.hpp>
#include <libremidi/ump_events.hpp>

#include <faust/dsp/poly-llvm-dsp.h>

namespace ossia::nodes
{

template <typename T>
struct faust_setup_ui : UI
{
  faust_setup_ui(T& self) { }
};

template <typename Node, bool Synth>
struct faust_exec_ui final : UI
{
  Node& fx;
  faust_exec_ui(Node& n)
      : fx{n}
  {
  }

  void addButton(const char* label, FAUSTFLOAT* zone) override
  {
    if constexpr(Synth)
    {
      using namespace std::literals;
      if(label == "Panic"sv || label == "gate"sv)
        return;
    }

    fx.root_inputs().push_back(new ossia::value_inlet);
    fx.controls.push_back(
        {fx.root_inputs().back()->template target<ossia::value_port>(), zone});
  }

  void addCheckButton(const char* label, FAUSTFLOAT* zone) override
  {
    addButton(label, zone);
  }

  void addVerticalSlider(
      const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min,
      FAUSTFLOAT max, FAUSTFLOAT step) override
  {
    if constexpr(Synth)
    {
      using namespace std::literals;
      if(label == "gain"sv || label == "freq"sv || label == "sustain"sv)
        return;
    }
    fx.root_inputs().push_back(new ossia::value_inlet);
    fx.controls.push_back(
        {fx.root_inputs().back()->template target<ossia::value_port>(), zone});
  }

  void addHorizontalSlider(
      const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min,
      FAUSTFLOAT max, FAUSTFLOAT step) override
  {
    addVerticalSlider(label, zone, init, min, max, step);
  }

  void addNumEntry(
      const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min,
      FAUSTFLOAT max, FAUSTFLOAT step) override
  {
    addVerticalSlider(label, zone, init, min, max, step);
  }

  void addHorizontalBargraph(
      const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) override
  {
    fx.root_outputs().push_back(new ossia::value_outlet);
    fx.displays.push_back(
        {fx.root_outputs().back()->template target<ossia::value_port>(), zone});
  }

  void addVerticalBargraph(
      const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) override
  {
    addHorizontalBargraph(label, zone, min, max);
  }

  void openTabBox(const char* label) override { }
  void openHorizontalBox(const char* label) override { }
  void openVerticalBox(const char* label) override { }
  void closeBox() override { }
  void declare(FAUSTFLOAT* zone, const char* key, const char* val) override { }
  void
  addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) override
  {
  }
};

template <typename Clone>
struct faust_exec_ui_clone final : ::UI
{
  Clone& self;
  int i = 0;
  int o = 0;
  faust_exec_ui_clone(Clone& s)
      : self{s}
  {
  }

  void addButton(const char* label, FAUSTFLOAT* zone) override
  {
    *zone = *self.controls[i].second;
    self.controls[i++].second = zone;
  }

  void addCheckButton(const char* label, FAUSTFLOAT* zone) override
  {
    addButton(label, zone);
  }

  void addVerticalSlider(
      const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min,
      FAUSTFLOAT max, FAUSTFLOAT step) override
  {
    *zone = *self.controls[i].second;
    self.controls[i++].second = zone;
  }

  void addHorizontalSlider(
      const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min,
      FAUSTFLOAT max, FAUSTFLOAT step) override
  {
    addVerticalSlider(label, zone, init, min, max, step);
  }

  void addNumEntry(
      const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min,
      FAUSTFLOAT max, FAUSTFLOAT step) override
  {
    addVerticalSlider(label, zone, init, min, max, step);
  }

  void addHorizontalBargraph(
      const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) override
  {
    self.displays[o++].second = zone;
  }

  void addVerticalBargraph(
      const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) override
  {
    addHorizontalBargraph(label, zone, min, max);
  }

  void openTabBox(const char* label) override { }
  void openHorizontalBox(const char* label) override { }
  void openVerticalBox(const char* label) override { }
  void closeBox() override { }
  void declare(FAUSTFLOAT* zone, const char* key, const char* val) override { }
  void
  addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) override
  {
  }
};

struct faust_node_utils
{
  template <typename Node>
  static void copy_controls(Node& self)
  {
    for(auto ctrl : self.controls)
    {
      auto& dat = ctrl.first->get_data();
      if(!dat.empty())
      {
        *ctrl.second = ossia::convert<float>(dat.back().value);
      }
    }
  }

  template <typename Node>
  static void copy_displays(Node& self, int64_t ts)
  {
    for(auto ctrl : self.displays)
    {
      ctrl.first->write_value(*ctrl.second, ts);
    }
  }

  template <typename Node>
  static void copy_input(
      Node& self, int64_t d, int64_t n_in, float* inputs_, float** input_n,
      const ossia::audio_port& audio_in)
  {
    // TODO offset !!!
    for(int64_t i = 0; i < n_in; i++)
    {
      input_n[i] = inputs_ + i * d;
      if(int64_t(audio_in.channels()) > i)
      {
        auto num_samples = std::min((int64_t)d, (int64_t)audio_in.channel(i).size());
        for(int64_t j = 0; j < num_samples; j++)
        {
          input_n[i][j] = (float)audio_in.channel(i)[j];
        }

        if(d > int64_t(audio_in.channel(i).size()))
        {
          for(int64_t j = audio_in.channel(i).size(); j < d; j++)
          {
            input_n[i][j] = 0.f;
          }
        }
      }
      else
      {
        for(int64_t j = 0; j < d; j++)
        {
          input_n[i][j] = 0.f;
        }
      }
    }
  }
  template <typename Node>
  static void copy_input_mono(
      Node& self, int64_t d, int64_t i, float* input,
      const ossia::audio_channel& audio_in)
  {
    // TODO offset !!!
    auto num_samples = std::min((int64_t)d, (int64_t)audio_in.size());
    for(int64_t j = 0; j < num_samples; j++)
    {
      input[j] = (float)audio_in[j];
    }

    if(d > int64_t(audio_in.size()))
    {
      for(int64_t j = audio_in.size(); j < d; j++)
      {
        input[j] = 0.f;
      }
    }
  }

  template <typename Node>
  static void
  init_output(Node& self, int64_t d, int64_t n_out, float* outputs_, float** output_n)
  {
    for(int64_t i = 0; i < n_out; i++)
    {
      output_n[i] = outputs_ + i * d;
      for(int64_t j = 0; j < d; j++)
      {
        output_n[i][j] = 0.f;
      }
    }
  }

  template <typename Node>
  static void copy_output(
      Node& self, int64_t d, int64_t n_out, float* outputs_, float** output_n,
      ossia::audio_port& audio_out)
  {
    audio_out.set_channels(n_out);
    for(int64_t i = 0; i < n_out; i++)
    {
      audio_out.channel(i).resize(d);
      for(int64_t j = 0; j < d; j++)
      {
        audio_out.channel(i)[j] = (double)output_n[i][j];
      }
    }

    // TODO handle multichannel cleanly
    if(n_out == 1)
    {
      audio_out.set_channels(2);
      audio_out.channel(1) = audio_out.channel(0);
    }
  }

  template <typename Node, typename Dsp>
  static void copy_midi(Node& self, Dsp& dsp, const ossia::midi_port& midi_in)
  {
    // TODO offset !!!

    for(const libremidi::ump& mess : midi_in.messages)
    {
      if(mess.get_type() != libremidi::midi2::message_type::MIDI_2_CHANNEL)
        continue;

      switch(libremidi::message_type(mess.get_status_code()))
      {
        case libremidi::message_type::NOTE_ON: {
          auto [channel, note, value] = libremidi::as_midi1::note_on(mess);
          self.in_flight[note]++;
          dsp.keyOn(channel, note, value);
          break;
        }
        case libremidi::message_type::NOTE_OFF: {
          auto [channel, note, value] = libremidi::as_midi1::note_off(mess);
          self.in_flight[note]--;
          dsp.keyOff(channel, note, value);
          break;
        }
        case libremidi::message_type::CONTROL_CHANGE: {
          auto [channel, index, value] = libremidi::as_midi1::control_change(mess);
          dsp.ctrlChange(channel, index, value);
          break;
        }
        case libremidi::message_type::PITCH_BEND: {
          auto [channel, value] = libremidi::as_midi1::pitch_bend(mess);
          dsp.pitchWheel(channel, value);
          break;
        }
        default:
          break;
          // TODO continue...
      }
    }
  }

  template <typename Node, typename DspPoly>
  void all_notes_off(Node& self, DspPoly& dsp)
  {
    for(int k = 0; k < 128; k++)
      while(self.in_flight[k]-- > 0)
        dsp.keyOff(1, k, 0);
  }

  /// Execution ///

  template <typename Node, typename Dsp>
  static void do_exec(
      Node& self, Dsp& dsp, const ossia::token_request& tk,
      const ossia::exec_state_facade& e)
  {
    const auto [st, d] = e.timings(tk);
    ossia::audio_port& audio_in
        = self.root_inputs()[0]->template cast<ossia::audio_port>();
    ossia::audio_port& audio_out
        = self.root_outputs()[0]->template cast<ossia::audio_port>();

    const int64_t n_in = dsp.getNumInputs();
    const int64_t n_out = dsp.getNumOutputs();
    audio_in.set_channels(n_in);
    audio_out.set_channels(n_out);

    if constexpr(std::is_same_v<FAUSTFLOAT, float>)
    {
      float* inputs_ = (float*)alloca(n_in * d * sizeof(float));
      float* outputs_ = (float*)alloca(n_out * d * sizeof(float));

      float** input_n = (float**)alloca(sizeof(float*) * n_in);
      float** output_n = (float**)alloca(sizeof(float*) * n_out);

      copy_input(self, d, n_in, inputs_, input_n, audio_in);
      init_output(self, d, n_out, outputs_, output_n);
      dsp.compute(d, input_n, output_n);
      copy_output(self, d, n_out, outputs_, output_n, audio_out);
    }
    else
    {
      double** input_n = (double**)alloca(sizeof(double*) * n_in);
      double** output_n = (double**)alloca(sizeof(double*) * n_out);
      for(int i = 0; i < n_in; i++)
      {
        audio_in.channel(i).resize(e.bufferSize());
        input_n[i] = audio_in.channel(i).data() + st;
      }
      if(BOOST_LIKELY(st == 0 && d == e.bufferSize()))
      {
        for(int i = 0; i < n_out; i++)
        {
          audio_out.channel(i).resize(e.bufferSize(), boost::container::default_init);
          output_n[i] = audio_out.channel(i).data() + st;
        }
      }
      else
      {
        for(int i = 0; i < n_out; i++)
        {
          audio_out.channel(i).resize(e.bufferSize());
          output_n[i] = audio_out.channel(i).data() + st;
        }
      }

      dsp.compute(d, input_n, output_n);
    }
  }

  template <typename Node, typename Dsp>
  static void exec(
      Node& self, Dsp& dsp, const ossia::token_request& tk,
      const ossia::exec_state_facade& e)
  {
    if(tk.forward())
    {
      const auto [st, d] = e.timings(tk);
      copy_controls(self);

      if(d == 0)
        return;
      do_exec(self, dsp, tk, e);
      copy_displays(self, st);
    }
  }

  template <typename Node, typename Dsp>
  static void do_exec_mono_fx(
      Node& self, Dsp& dsp, const ossia::token_request& tk,
      const ossia::exec_state_facade& e)
  {
    const auto [st, d] = e.timings(tk);
    if(d == 0)
      return;

    ossia::audio_port& audio_in
        = self.root_inputs()[0]->template cast<ossia::audio_port>();
    ossia::audio_port& audio_out
        = self.root_outputs()[0]->template cast<ossia::audio_port>();

    const int64_t n_in = audio_in.channels();
    audio_out.set_channels(n_in);
    while(self.clones.size() < n_in)
    {
      self.clones.emplace_back(dsp.clone(), self.clones[0]);
    }

    {
      for(int k = 0; k < self.controls.size(); ++k)
      {
        auto ctrl = self.controls[k];
        auto& dat = ctrl.first->get_data();
        if(!dat.empty())
        {
          if(dat.back().value.valid())
            ossia::apply_nonnull([k,&self] (const auto& vv){ self.set_control(k, vv); }, dat.back().value.v);
        }
      }
    }

    if constexpr(std::is_same_v<FAUSTFLOAT, float>)
    {
      float* input = (float*)alloca(d * sizeof(float));
      memset(input, 0, d * sizeof(float));
      float* output = (float*)alloca(d * sizeof(float));

      for(int i = 0; i < n_in; i++)
      {
        auto& in_chan = audio_in.channel(i);
        auto& out_chan = audio_out.channel(i);
        auto& clone = self.clones[i];
        in_chan.resize(e.bufferSize());
        out_chan.resize(e.bufferSize());

        copy_input_mono(self, d, n_in, input, in_chan);
        memset(output, 0, d * sizeof(float));
        for(int z = 0; z < d; z++)
        {
          assert(!std::isnan(input[z]));
          assert(!std::isinf(input[z]));
        }
        clone.fx->compute(d, &input, &output);
        for(int z = 0; z < d; z++)
        {
          if(std::fpclassify(output[z]) != FP_NORMAL)
            output[z] = 0.f;
        }

        std::copy_n(output, d, out_chan.data() + st);
        for(int z = 0; z < e.bufferSize(); z++)
        {
          assert(!std::isnan(out_chan[z]));
          assert(!std::isinf(out_chan[z]));
        }
      }
    }
    else
    {
      for(int i = 0; i < n_in; i++)
      {
        auto& in_chan = audio_in.channel(i);
        auto& out_chan = audio_out.channel(i);
        in_chan.resize(e.bufferSize());
        out_chan.resize(e.bufferSize());

        double* input = in_chan.data() + st;
        double* output = out_chan.data() + st;

        self.clones[i].fx->compute(d, &input, &output);
      }
    }
  }

  template <typename Node, typename Dsp>
  static void exec_mono_fx(
      Node& self, Dsp& dsp, const ossia::token_request& tk,
      const ossia::exec_state_facade& e)
  {
    if(tk.forward())
    {
      const auto [st, d] = e.timings(tk);

      if(d == 0)
        return;
      do_exec_mono_fx(self, dsp, tk, e);
      copy_displays(self, st);
    }
  }

  /// Synth ///
  template <typename Node, typename DspPoly>
  void exec_synth(
      Node& self, DspPoly& dsp, const ossia::token_request& tk,
      const ossia::exec_state_facade& e)
  {
    if(tk.forward())
    {
      const auto [st, d] = e.timings(tk);

      auto& midi_in = self.root_inputs()[1]->template cast<ossia::midi_port>();

      copy_controls(self);
      dsp.updateAllZones();
      copy_midi(self, dsp, midi_in);

      if(d == 0)
        return;
      do_exec(self, dsp, tk, e);
      copy_displays(self, st);
    }
  }
};

// NOTE: all the code below taken and slightly modified from poly-dsp
// with small modifications to allow thread-safe update of GroupUI
// Keep up-to-date with updates in faust.. last update 2020-10-18
// GPLv3
// Copyright is the Faust team and Faust contributors
// Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
class custom_dsp_poly_effect : public dsp_poly
{

private:
  mydsp_poly* fPolyDSP;

public:
  custom_dsp_poly_effect(mydsp_poly* dsp1, dsp* dsp2)
      : dsp_poly(dsp2)
      , fPolyDSP(dsp1)
  {
  }

  virtual ~custom_dsp_poly_effect()
  {
    // dsp_poly_effect is also a decorator_dsp, which will free fPolyDSP
  }

  void updateAllZones() { fPolyDSP->fGroups.updateAllZones(); }
  // MIDI API
  MapUI* keyOn(int channel, int pitch, int velocity)
  {
    return fPolyDSP->keyOn(channel, pitch, velocity);
  }
  void keyOff(int channel, int pitch, int velocity)
  {
    fPolyDSP->keyOff(channel, pitch, velocity);
  }
  void keyPress(int channel, int pitch, int press)
  {
    fPolyDSP->keyPress(channel, pitch, press);
  }
  void chanPress(int channel, int press) { fPolyDSP->chanPress(channel, press); }
  void ctrlChange(int channel, int ctrl, int value)
  {
    fPolyDSP->ctrlChange(channel, ctrl, value);
  }
  void ctrlChange14bits(int channel, int ctrl, int value)
  {
    fPolyDSP->ctrlChange14bits(channel, ctrl, value);
  }
  void pitchWheel(int channel, int wheel) { fPolyDSP->pitchWheel(channel, wheel); }
  void progChange(int channel, int pgm) { fPolyDSP->progChange(channel, pgm); }
};

struct custom_dsp_poly_factory : public dsp_factory
{
  dsp_factory* fProcessFactory;
  dsp_factory* fEffectFactory;

  std::vector<std::string> getWarningMessages() { return {}; }

  std::string getEffectCode(const std::string& dsp_content)
  {
    std::stringstream effect_code;
    effect_code << "adapt(1,1) = _; adapt(2,2) = _,_; adapt(1,2) = _ <: _,_; "
                   "adapt(2,1) = _,_ :> _;";
    effect_code << "adaptor(F,G) = adapt(outputs(F),inputs(G)); dsp_code = "
                   "environment{ "
                << dsp_content << " };";
    effect_code << "process = adaptor(dsp_code.process, dsp_code.effect) : "
                   "dsp_code.effect;";
    return effect_code.str();
  }

  custom_dsp_poly_factory(
      dsp_factory* process_factory = NULL, dsp_factory* effect_factory = NULL)
      : fProcessFactory(process_factory)
      , fEffectFactory(effect_factory)
  {
  }

  virtual ~custom_dsp_poly_factory() = default;

  virtual std::string getName() { return fProcessFactory->getName(); }
  virtual std::string getSHAKey() { return fProcessFactory->getSHAKey(); }
  virtual std::string getDSPCode() { return fProcessFactory->getDSPCode(); }
  virtual std::string getCompileOptions()
  {
    return fProcessFactory->getCompileOptions();
  }
  virtual std::vector<std::string> getLibraryList()
  {
    return fProcessFactory->getLibraryList();
  }
  virtual std::vector<std::string> getIncludePathnames()
  {
    return fProcessFactory->getIncludePathnames();
  }

  virtual void setMemoryManager(dsp_memory_manager* manager)
  {
    fProcessFactory->setMemoryManager(manager);
    if(fEffectFactory)
    {
      fEffectFactory->setMemoryManager(manager);
    }
  }
  virtual dsp_memory_manager* getMemoryManager()
  {
    return fProcessFactory->getMemoryManager();
  }

  /* Create a new polyphonic DSP instance with global effect, to be deleted
   * with C++ 'delete'
   *
   * @param nvoices - number of polyphony voices, should be at least 1
   * @param control - whether voices will be dynamically allocated and
   * controlled (typically by a MIDI controller). If false all voices are always
   * running.
   * @param group - if true, voices are not individually accessible, a global
   * "Voices" tab will automatically dispatch a given control on all voices,
   * assuming GUI::updateAllGuis() is called. If false, all voices can be
   * individually controlled.
   */
  custom_dsp_poly_effect* createPolyDSPInstance(int nvoices, bool control, bool group)
  {
    auto dsp_poly
        = new mydsp_poly(fProcessFactory->createDSPInstance(), nvoices, control, group);
    if(fEffectFactory)
    {
      // the 'dsp_poly' object has to be controlled with MIDI, so kept
      // separated from new dsp_sequencer(...) object
      return new custom_dsp_poly_effect(
          dsp_poly, new dsp_sequencer(dsp_poly, fEffectFactory->createDSPInstance()));
    }
    else
    {
      return new custom_dsp_poly_effect(dsp_poly, dsp_poly);
    }
  }

  /* Create a new DSP instance, to be deleted with C++ 'delete' */
  dsp* createDSPInstance() { return fProcessFactory->createDSPInstance(); }
};
struct custom_llvm_dsp_poly_factory : public custom_dsp_poly_factory
{
  custom_llvm_dsp_poly_factory(
      const std::string& name_app, const std::string& dsp_content, int argc,
      const char* argv[], const std::string& target, std::string& error_msg,
      int opt_level = -1)
  {
    fProcessFactory = createDSPFactoryFromString(
        name_app, dsp_content, argc, argv, target, error_msg);
    if(fProcessFactory)
    {
      fEffectFactory = createDSPFactoryFromString(
          name_app, getEffectCode(dsp_content), argc, argv, target, error_msg);
      if(!fEffectFactory)
      {
        std::cerr << "llvm_dsp_poly_factory : fEffectFactory " << error_msg;
        // The error message is not really needed...
        error_msg = "";
      }
    }
    else
    {
      std::cerr << "llvm_dsp_poly_factory : fProcessFactory " << error_msg;
      throw std::bad_alloc();
    }
  }

  virtual ~custom_llvm_dsp_poly_factory()
  {
    deleteDSPFactory(static_cast<llvm_dsp_factory*>(fProcessFactory));
    deleteDSPFactory(static_cast<llvm_dsp_factory*>(fEffectFactory));
  }
};

static custom_llvm_dsp_poly_factory* createCustomPolyDSPFactoryFromString(
    const std::string& name_app, const std::string& dsp_content, int argc,
    const char* argv[], const std::string& target, std::string& error_msg,
    int opt_level = -1)
{
  try
  {
    return new custom_llvm_dsp_poly_factory(
        name_app, dsp_content, argc, argv, target, error_msg, opt_level);
  }
  catch(...)
  {
    return NULL;
  }
}

}
