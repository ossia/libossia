#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>

#if __has_include(<faust/dsp/poly-llvm-dsp.h>)
#include <faust/dsp/poly-llvm-dsp.h>
#else
#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

struct Soundfile { };
struct Meta { void declare(const char*, const char*) { } };
struct dsp { };
struct UI
{
  virtual ~UI() = default;

  virtual void openTabBox(const char* label) = 0;
  virtual void openHorizontalBox(const char* label) = 0;
  virtual void openVerticalBox(const char* label) = 0;
  virtual void closeBox() = 0;
  virtual void declare(FAUSTFLOAT* zone, const char* key, const char* val) = 0;
  virtual void addSoundfile(
      const char* label,
      const char* filename,
      Soundfile** sf_zone) = 0;
  virtual void addButton(const char* label, FAUSTFLOAT* zone) = 0;
  virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) = 0;
  virtual void addVerticalSlider(
      const char* label,
      FAUSTFLOAT* zone,
      FAUSTFLOAT init,
      FAUSTFLOAT min,
      FAUSTFLOAT max,
      FAUSTFLOAT step) = 0;
  virtual void addHorizontalSlider(
      const char* label,
      FAUSTFLOAT* zone,
      FAUSTFLOAT init,
      FAUSTFLOAT min,
      FAUSTFLOAT max,
      FAUSTFLOAT step) = 0;
  virtual void addNumEntry(
      const char* label,
      FAUSTFLOAT* zone,
      FAUSTFLOAT init,
      FAUSTFLOAT min,
      FAUSTFLOAT max,
      FAUSTFLOAT step) = 0;
  virtual void addHorizontalBargraph(
      const char* label,
      FAUSTFLOAT* zone,
      FAUSTFLOAT min,
      FAUSTFLOAT max) = 0;
  virtual void addVerticalBargraph(
      const char* label,
      FAUSTFLOAT* zone,
      FAUSTFLOAT min,
      FAUSTFLOAT max) = 0;
};

#endif

namespace ossia::nodes
{

template <typename T>
struct faust_setup_ui : UI
{
  faust_setup_ui(T& self)
  {
  }
};

template <typename Node, bool Synth>
struct faust_exec_ui final : UI
{
  Node& fx;
  faust_exec_ui(Node& n) : fx{n}
  {
  }

  void addButton(const char* label, FAUSTFLOAT* zone) override
  {
    if constexpr(Synth)
    {
      using namespace std::literals;
      if (label == "Panic"sv || label == "gate"sv)
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
      if (label == "gain"sv || label == "freq"sv || label == "sustain"sv)
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

  void openTabBox(const char* label) override
  {
  }
  void openHorizontalBox(const char* label) override
  {
  }
  void openVerticalBox(const char* label) override
  {
  }
  void closeBox() override
  {
  }
  void declare(FAUSTFLOAT* zone, const char* key, const char* val) override
  {
  }
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
    for (auto ctrl : self.controls)
    {
      auto& dat = ctrl.first->get_data();
      if (!dat.empty())
      {
        *ctrl.second = ossia::convert<float>(dat.back().value);
      }
    }
  }

  template <typename Node>
  static void copy_displays(Node& self, int64_t ts)
  {
    for (auto ctrl : self.displays)
    {
      ctrl.first->write_value(*ctrl.second, ts);
    }
  }

  template <typename Node>
  static void copy_input(Node& self, int64_t d, int64_t n_in, float* inputs_, float** input_n, const ossia::audio_port& audio_in)
  {
    // TODO offset !!!
    for (int64_t i = 0; i < n_in; i++)
    {
      input_n[i] = inputs_ + i * d;
      if (int64_t(audio_in.samples.size()) > i)
      {
        auto num_samples = std::min(
              (int64_t)d, (int64_t)audio_in.samples[i].size());
        for (int64_t j = 0; j < num_samples; j++)
        {
          input_n[i][j] = (float)audio_in.samples[i][j];
        }

        if (d > int64_t(audio_in.samples[i].size()))
        {
          for (int64_t j = audio_in.samples[i].size(); j < d; j++)
          {
            input_n[i][j] = 0.f;
          }
        }
      }
      else
      {
        for (int64_t j = 0; j < d; j++)
        {
          input_n[i][j] = 0.f;
        }
      }
    }
  }

  template <typename Node>
  static void init_output(Node& self, int64_t d, int64_t n_out, float* outputs_, float** output_n)
  {
    for (int64_t i = 0; i < n_out; i++)
    {
      output_n[i] = outputs_ + i * d;
      for (int64_t j = 0; j < d; j++)
      {
        output_n[i][j] = 0.f;
      }
    }
  }

  template <typename Node>
  static void copy_output(Node& self, int64_t d, int64_t n_out, float* outputs_, float** output_n, ossia::audio_port& audio_out)
  {
    audio_out.samples.resize(n_out);
    for (int64_t i = 0; i < n_out; i++)
    {
      audio_out.samples[i].resize(d);
      for (int64_t j = 0; j < d; j++)
      {
        audio_out.samples[i][j] = (double)output_n[i][j];
      }
    }

    // TODO handle multichannel cleanly
    if (n_out == 1)
    {
      audio_out.samples.resize(2);
      audio_out.samples[1] = audio_out.samples[0];
    }
  }

  template <typename Node, typename Dsp>
  static void copy_midi(Node& self, Dsp& dsp, const ossia::midi_port& midi_in)
  {
    // TODO offset !!!

    for(const rtmidi::message& mess : midi_in.messages)
    {
      switch(mess.get_message_type())
      {
        case rtmidi::message_type::NOTE_ON:
        {
          dsp.keyOn(mess[0], mess[1], mess[2]);
          break;
        }
        case rtmidi::message_type::NOTE_OFF:
        {
          dsp.keyOff(mess[0], mess[1], mess[2]);
          break;
        }
        case rtmidi::message_type::CONTROL_CHANGE:
        {
          dsp.ctrlChange(mess[0], mess[1], mess[2]);
          break;
        }
        case rtmidi::message_type::PITCH_BEND:
        {
          dsp.pitchWheel(mess[0], mess.bytes[2] * 128 + mess.bytes[1]);
          break;
        }
        default:
          break;
          // TODO continue...
      }
    }
  }

  /// Execution ///
  template <typename Node, typename Dsp>
  static void exec(Node& self, Dsp& dsp, const ossia::token_request& tk, const ossia::exec_state_facade& e)
  {
    if (tk.forward())
    {
      const int64_t st = tk.physical_start(e.modelToSamples());
      const int64_t d = tk.physical_write_duration(e.modelToSamples());

      auto& audio_in = self.root_inputs()[0]->template cast<ossia::audio_port>();
      auto& audio_out = self.root_outputs()[0]->template cast<ossia::audio_port>();
      const int64_t n_in = dsp.getNumInputs();
      const int64_t n_out = dsp.getNumOutputs();

      float* inputs_ = (float*)alloca(n_in * d * sizeof(float));
      float* outputs_ = (float*)alloca(n_out * d * sizeof(float));

      float** input_n = (float**)alloca(sizeof(float*) * n_in);
      float** output_n = (float**)alloca(sizeof(float*) * n_out);

      copy_controls(self);

      copy_input(self, d, n_in, inputs_, input_n, audio_in);
      init_output(self, d, n_out, outputs_, output_n);
      dsp.compute(d, input_n, output_n);
      copy_output(self, d, n_out, outputs_, output_n, audio_out);

      copy_displays(self, st);
    }
  }

  /// Synth ///
  template <typename Node, typename DspPoly>
  void exec_synth(Node& self, DspPoly& dsp, const ossia::token_request& tk, const ossia::exec_state_facade& e)
  {
    if (tk.forward())
    {
      const int64_t st = tk.physical_start(e.modelToSamples());
      const int64_t d = tk.physical_write_duration(e.modelToSamples());

      auto& audio_in = self.root_inputs()[0]->template cast<ossia::audio_port>();
      auto& midi_in = self.root_inputs()[1]->template cast<ossia::midi_port>();
      auto& audio_out = self.root_outputs()[0]->template cast<ossia::audio_port>();

      const int64_t n_in = dsp.getNumInputs();
      const int64_t n_out = dsp.getNumOutputs();

      float* inputs_ = (float*)alloca(n_in * d * sizeof(float));
      float* outputs_ = (float*)alloca(n_out * d * sizeof(float));

      float** input_n = (float**)alloca(sizeof(float*) * n_in);
      float** output_n = (float**)alloca(sizeof(float*) * n_out);

      copy_controls(self);
      dsp.updateAllZones();

      copy_midi(self, dsp, midi_in);

      copy_input(self, d, n_in, inputs_, input_n, audio_in);
      init_output(self, d, n_out, outputs_, output_n);
      dsp.compute(d, input_n, output_n);
      copy_output(self, d, n_out, outputs_, output_n, audio_out);

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
class custom_dsp_poly_effect : public dsp_poly {

private:
  mydsp_poly* fPolyDSP;

public:
  custom_dsp_poly_effect(mydsp_poly* dsp1, dsp* dsp2)
    :dsp_poly(dsp2), fPolyDSP(dsp1)
  {}

  virtual ~custom_dsp_poly_effect()
  {
    // dsp_poly_effect is also a decorator_dsp, which will free fPolyDSP
  }

  void updateAllZones()
  {
    fPolyDSP->fGroups.updateAllZones();
  }
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
  void chanPress(int channel, int press)
  {
    fPolyDSP->chanPress(channel, press);
  }
  void ctrlChange(int channel, int ctrl, int value)
  {
    fPolyDSP->ctrlChange(channel, ctrl, value);
  }
  void ctrlChange14bits(int channel, int ctrl, int value)
  {
    fPolyDSP->ctrlChange14bits(channel, ctrl, value);
  }
  void pitchWheel(int channel, int wheel)
  {
    fPolyDSP->pitchWheel(channel, wheel);
  }
  void progChange(int channel, int pgm)
  {
    fPolyDSP->progChange(channel, pgm);
  }

  // Group API
  void setGroup(bool group)
  {
    fPolyDSP->setGroup(group);
  }
  bool getGroup()
  {
    return fPolyDSP->getGroup();
  }
};

struct custom_dsp_poly_factory : public dsp_factory {

  dsp_factory* fProcessFactory;
  dsp_factory* fEffectFactory;

  std::string getEffectCode(const std::string& dsp_content)
  {
    std::stringstream effect_code;
    effect_code << "adapt(1,1) = _; adapt(2,2) = _,_; adapt(1,2) = _ <: _,_; adapt(2,1) = _,_ :> _;";
    effect_code << "adaptor(F,G) = adapt(outputs(F),inputs(G)); dsp_code = environment{ " << dsp_content << " };";
    effect_code << "process = adaptor(dsp_code.process, dsp_code.effect) : dsp_code.effect;";
    return effect_code.str();
  }

  custom_dsp_poly_factory(dsp_factory* process_factory = NULL,
                          dsp_factory* effect_factory = NULL):
    fProcessFactory(process_factory)
  ,fEffectFactory(effect_factory)
  {}

  virtual ~custom_dsp_poly_factory()
  {}

  virtual std::string getName() { return fProcessFactory->getName(); }
  virtual std::string getSHAKey() { return fProcessFactory->getSHAKey(); }
  virtual std::string getDSPCode() { return fProcessFactory->getDSPCode(); }
  virtual std::string getCompileOptions() { return fProcessFactory->getCompileOptions(); }
  virtual std::vector<std::string> getLibraryList() { return fProcessFactory->getLibraryList(); }
  virtual std::vector<std::string> getIncludePathnames() { return fProcessFactory->getIncludePathnames(); }

  virtual void setMemoryManager(dsp_memory_manager* manager)
  {
    fProcessFactory->setMemoryManager(manager);
    if (fEffectFactory) {
      fEffectFactory->setMemoryManager(manager);
    }
  }
  virtual dsp_memory_manager* getMemoryManager() { return fProcessFactory->getMemoryManager(); }

  /* Create a new polyphonic DSP instance with global effect, to be deleted with C++ 'delete'
     *
     * @param nvoices - number of polyphony voices, should be at least 1
     * @param control - whether voices will be dynamically allocated and controlled (typically by a MIDI controler).
     *                If false all voices are always running.
     * @param group - if true, voices are not individually accessible, a global "Voices" tab will automatically dispatch
     *                a given control on all voices, assuming GUI::updateAllGuis() is called.
     *                If false, all voices can be individually controlled.
     */
  custom_dsp_poly_effect* createPolyDSPInstance(int nvoices, bool control, bool group)
  {
    auto dsp_poly = new mydsp_poly(fProcessFactory->createDSPInstance(), nvoices, control, group);
    if (fEffectFactory) {
      // the 'dsp_poly' object has to be controlled with MIDI, so kept separated from new dsp_sequencer(...) object
      return new custom_dsp_poly_effect(dsp_poly, new dsp_sequencer(dsp_poly, fEffectFactory->createDSPInstance()));
    } else {
      return new custom_dsp_poly_effect(dsp_poly, dsp_poly);
    }
  }

  /* Create a new DSP instance, to be deleted with C++ 'delete' */
  dsp* createDSPInstance()
  {
    return fProcessFactory->createDSPInstance();
  }

};
struct custom_llvm_dsp_poly_factory : public custom_dsp_poly_factory {

  custom_llvm_dsp_poly_factory(const std::string& name_app,
                               const std::string& dsp_content,
                               int argc, const char* argv[],
                               const std::string& target,
                               std::string& error_msg,
                               int opt_level = -1)
  {
    fProcessFactory = createDSPFactoryFromString(name_app, dsp_content, argc, argv, target, error_msg);
    if (fProcessFactory) {
      fEffectFactory = createDSPFactoryFromString(name_app, getEffectCode(dsp_content), argc, argv, target, error_msg);
      if (!fEffectFactory) {
        std::cerr << "llvm_dsp_poly_factory : fEffectFactory " << error_msg;
        // The error message is not really needed...
        error_msg = "";
      }
    } else {
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

static custom_llvm_dsp_poly_factory* createCustomPolyDSPFactoryFromString(const std::string& name_app,
                                                                          const std::string& dsp_content,
                                                                          int argc, const char* argv[],
                                                                          const std::string& target,
                                                                          std::string& error_msg,
                                                                          int opt_level = -1)
{
  try {
    return new custom_llvm_dsp_poly_factory(name_app, dsp_content, argc, argv, target, error_msg, opt_level);
  } catch (...) {
    return NULL;
  }
}

}
