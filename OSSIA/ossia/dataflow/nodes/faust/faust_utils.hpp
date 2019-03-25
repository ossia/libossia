#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>

#include <faust/dsp/llvm-c-dsp.h>
#include <faust/midi/midi.h>

namespace ossia::nodes
{

template <typename T>
struct faust_setup_ui : UIGlue
{
  faust_setup_ui(T& self)
  {
    uiInterface = &self;

    openTabBox = [](void* self, const char* arg1) {
      return reinterpret_cast<T*>(self)->openTabBox(arg1);
    };
    openHorizontalBox = [](void* self, const char* arg1) {
      return reinterpret_cast<T*>(self)->openHorizontalBox(arg1);
    };
    openVerticalBox = [](void* self, const char* arg1) {
      return reinterpret_cast<T*>(self)->openVerticalBox(arg1);
    };
    closeBox
        = [](void* self) { return reinterpret_cast<T*>(self)->closeBox(); };
    addButton = [](void* self, const char* label, FAUSTFLOAT* zone) {
      return reinterpret_cast<T*>(self)->addButton(label, zone);
    };
    addCheckButton = [](void* self, const char* label, FAUSTFLOAT* zone) {
      return reinterpret_cast<T*>(self)->addCheckButton(label, zone);
    };
    addVerticalSlider
        = [](void* self, const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init,
             FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {
            return reinterpret_cast<T*>(self)->addVerticalSlider(
                label, zone, init, min, max, step);
          };
    addHorizontalSlider
        = [](void* self, const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init,
             FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {
            return reinterpret_cast<T*>(self)->addHorizontalSlider(
                label, zone, init, min, max, step);
          };
    addNumEntry
        = [](void* self, const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init,
             FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {
            return reinterpret_cast<T*>(self)->addNumEntry(
                label, zone, init, min, max, step);
          };
    addHorizontalBargraph = [](void* self, const char* label, FAUSTFLOAT* zone,
                               FAUSTFLOAT min, FAUSTFLOAT max) {
      return reinterpret_cast<T*>(self)->addHorizontalBargraph(
          label, zone, min, max);
    };
    addVerticalBargraph = [](void* self, const char* label, FAUSTFLOAT* zone,
                             FAUSTFLOAT min, FAUSTFLOAT max) {
      return reinterpret_cast<T*>(self)->addVerticalBargraph(
          label, zone, min, max);
    };
    addSoundfile = [](void* self, const char* label, const char* filename,
                      Soundfile** sf_zone) {
      return reinterpret_cast<T*>(self)->addSoundfile(
          label, filename, sf_zone);
    };
    declare
        = [](void* self, FAUSTFLOAT* zone, const char* key, const char* val) {
            return reinterpret_cast<T*>(self)->declare(zone, key, val);
          };
  }
};

template <typename Node>
struct faust_exec_ui final
{
  Node& fx;
  faust_setup_ui<faust_exec_ui> glue{*this};
  faust_exec_ui(Node& n) : fx{n}
  {
  }

  void addButton(const char* label, FAUSTFLOAT* zone)
  {
    fx.inputs().push_back(ossia::make_inlet<ossia::value_port>());
    fx.controls.push_back(
        {fx.inputs().back()->data.template target<ossia::value_port>(), zone});
  }

  void addCheckButton(const char* label, FAUSTFLOAT* zone)
  {
    addButton(label, zone);
  }

  void addVerticalSlider(
      const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min,
      FAUSTFLOAT max, FAUSTFLOAT step)
  {
    fx.inputs().push_back(ossia::make_inlet<ossia::value_port>());
    fx.controls.push_back(
        {fx.inputs().back()->data.template target<ossia::value_port>(), zone});
  }

  void addHorizontalSlider(
      const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min,
      FAUSTFLOAT max, FAUSTFLOAT step)
  {
    addVerticalSlider(label, zone, init, min, max, step);
  }

  void addNumEntry(
      const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min,
      FAUSTFLOAT max, FAUSTFLOAT step)
  {
    addVerticalSlider(label, zone, init, min, max, step);
  }

  void addHorizontalBargraph(
      const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
  {
    fx.outputs().push_back(ossia::make_outlet<ossia::value_port>());
  }

  void addVerticalBargraph(
      const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
  {
    addHorizontalBargraph(label, zone, min, max);
  }

  void openTabBox(const char* label)
  {
  }
  void openHorizontalBox(const char* label)
  {
  }
  void openVerticalBox(const char* label)
  {
  }
  void closeBox()
  {
  }
  void declare(FAUSTFLOAT* zone, const char* key, const char* val)
  {
  }
  void
  addSoundfile(const char* label, const char* filename, Soundfile** sf_zone)
  {
  }
};


/// Execution ///
template <typename Node, typename Dsp>
void faust_exec(Node& self, Dsp& dsp, const ossia::token_request& tk)
{
  if (tk.date > tk.prev_date)
  {
    std::size_t d = tk.date - tk.prev_date;
    for (auto ctrl : self.controls)
    {
      auto& dat = ctrl.first->get_data();
      if (!dat.empty())
      {
        *ctrl.second = ossia::convert<float>(dat.back().value);
      }
    }

    auto& audio_in
        = *self.inputs()[0]->data.template target<ossia::audio_port>();
    auto& audio_out
        = *self.outputs()[0]->data.template target<ossia::audio_port>();

    const std::size_t n_in = dsp.getNumInputs();
    const std::size_t n_out = dsp.getNumOutputs();

    float* inputs_ = (float*)alloca(n_in * d * sizeof(float));
    float* outputs_ = (float*)alloca(n_out * d * sizeof(float));

    float** input_n = (float**)alloca(sizeof(float*) * n_in);
    float** output_n = (float**)alloca(sizeof(float*) * n_out);

    // Copy inputs
    // TODO offset !!!
    for (std::size_t i = 0; i < n_in; i++)
    {
      input_n[i] = inputs_ + i * d;
      if (audio_in.samples.size() > i)
      {
        auto num_samples = std::min(
            (std::size_t)d, (std::size_t)audio_in.samples[i].size());
        for (std::size_t j = 0; j < num_samples; j++)
        {
          input_n[i][j] = (float)audio_in.samples[i][j];
        }

        if (d > audio_in.samples[i].size())
        {
          for (std::size_t j = audio_in.samples[i].size(); j < d; j++)
          {
            input_n[i][j] = 0.f;
          }
        }
      }
      else
      {
        for (std::size_t j = 0; j < d; j++)
        {
          input_n[i][j] = 0.f;
        }
      }
    }

    for (std::size_t i = 0; i < n_out; i++)
    {
      output_n[i] = outputs_ + i * d;
      for (std::size_t j = 0; j < d; j++)
      {
        output_n[i][j] = 0.f;
      }
    }

    dsp.compute(d, input_n, output_n);

    audio_out.samples.resize(n_out);
    for (std::size_t i = 0; i < n_out; i++)
    {
      audio_out.samples[i].resize(d);
      for (std::size_t j = 0; j < d; j++)
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
}

/// Synth ///


//-----------------------------------------------
// MIDI input/output handling using JUCE library
//-----------------------------------------------

class juce_midi_handler : public midi_handler {

    protected:

        ossia::midi_port fOutputBuffer;
/*
        void decodeMessage(const MidiMessage& message)
        {
            const uint8* data = message.getRawData();

            if (message.isNoteOff()) {
                for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                    fMidiInputs[i]->keyOff(message.getTimeStamp(), message.getChannel(), data[1], data[2]);
                }
            } else if (message.isNoteOn()) {
                for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                    if (data[1] != 0) {
                        fMidiInputs[i]->keyOn(message.getTimeStamp(), message.getChannel(), data[1], data[2]);
                    } else {
                        fMidiInputs[i]->keyOff(message.getTimeStamp(), message.getChannel(), data[1], data[2]);
                    }
                }
            } else if (message.isAftertouch()) {
                for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                    fMidiInputs[i]->keyPress(message.getTimeStamp(), message.getChannel(), data[1], data[2]);
                }
            } else if (message.isController()) {
                for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                    fMidiInputs[i]->ctrlChange(message.getTimeStamp(), message.getChannel(), data[1], data[2]);
                }
            } else if (message.isProgramChange()) {
                for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                    fMidiInputs[i]->progChange(message.getTimeStamp(), message.getChannel(), data[1]);
                }
            } else if (message.isChannelPressure()) {
                for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                    fMidiInputs[i]->chanPress(message.getTimeStamp(), message.getChannel(), data[1]);
                }
            } else if (message.isPitchWheel()) {
                for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                    fMidiInputs[i]->pitchWheel(message.getTimeStamp(), message.getChannel(), ((data[1] << 7) + data[2]));
                }
            } else if (message.isMidiClock()) {
                for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                    fMidiInputs[i]->clock(message.getTimeStamp());
                }
            } else if (message.isMidiStart()) {
                for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                    fMidiInputs[i]->startSync(message.getTimeStamp());
                }
            } else if (message.isMidiStop()) {
                for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                    fMidiInputs[i]->stopSync(message.getTimeStamp());
                }
            } else if (message.isSysEx()) {
                std::vector<unsigned char> sysex(data, data + message.getRawDataSize());
                for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                    fMidiInputs[i]->sysEx(message.getTimeStamp(), sysex);
                }
            } else {
                std::cerr << "Unused MIDI message" << std::endl;
            }
        }
*/
    public:

        juce_midi_handler():midi_handler("JUCE")
        {}

        virtual ~juce_midi_handler() {}
/*
        // Used with MidiBuffer (containing several messages)
        void encodeBuffer(MidiBuffer& buffer)
        {
            const ScopedTryLock lock(fMutex);
            if (lock.isLocked()) {
                buffer.swapWith(fOutputBuffer);
                fOutputBuffer.clear();
            } else {
                std::cerr << "encodeBuffer fails..." << std::endl;
            }
        }

        void decodeBuffer(MidiBuffer& buffer)
        {
            MidiMessage msg;
            int ignore;

            for (MidiBuffer::Iterator it(buffer); it.getNextEvent(msg, ignore);) {
                decodeMessage(msg);
            }

            buffer.clear();
        }
*/
        // MIDI output API

        bool startMidi()
        {
            return true;
        }

        void stopMidi()
        {
        }

        MapUI* keyOn(int channel, int pitch, int velocity)
        {
            return nullptr;
        }

        void keyOff(int channel, int pitch, int velocity)
        {
        }

        void ctrlChange(int channel, int ctrl, int val)
        {
        }

        void chanPress(int channel, int press)
        {
        }

        void progChange(int channel, int pgm)
        {
        }

        void keyPress(int channel, int pitch, int press)
        {
        }

        void pitchWheel(int channel, int wheel)
        {
        }

        void ctrlChange14bits(int channel, int ctrl, int value)
        {
        }

        void startSync(double date)
        {
        }

        void stopSync(double date)
        {
        }

        void clock(double date)
        {
        }

        void sysEx(double date, std::vector<unsigned char>& message)
        {
        }

};

class juce_midi : public juce_midi_handler {

    public:

        virtual ~juce_midi()
        {
        }

};

template <typename Node, typename Dsp>
void faust_exec_synth(Node& self, Dsp& dsp, const ossia::token_request& tk)
{
  if (tk.date > tk.prev_date)
  {
    std::size_t d = tk.date - tk.prev_date;
    for (auto ctrl : self.controls)
    {
      auto& dat = ctrl.first->get_data();
      if (!dat.empty())
      {
        *ctrl.second = ossia::convert<float>(dat.back().value);
      }
    }

    auto& midi_in
        = *self.inputs()[0]->data.template target<ossia::midi_port>();
    auto& audio_out
        = *self.outputs()[0]->data.template target<ossia::audio_port>();

    const std::size_t n_in = dsp.getNumInputs();
    const std::size_t n_out = dsp.getNumOutputs();

    float* inputs_ = (float*)alloca(n_in * d * sizeof(float));
    float* outputs_ = (float*)alloca(n_out * d * sizeof(float));

    float** input_n = (float**)alloca(sizeof(float*) * n_in);
    float** output_n = (float**)alloca(sizeof(float*) * n_out);

    // Copy inputs
    // TODO offset !!!
    for (std::size_t i = 0; i < n_in; i++)
    {
      input_n[i] = inputs_ + i * d;
      if (midi_in.samples.size() > i)
      {
        auto num_samples = std::min(
            (std::size_t)d, (std::size_t)midi_in.samples[i].size());
        for (std::size_t j = 0; j < num_samples; j++)
        {
          input_n[i][j] = (float)midi_in.samples[i][j];
        }

        if (d > midi_in.samples[i].size())
        {
          for (std::size_t j = midi_in.samples[i].size(); j < d; j++)
          {
            input_n[i][j] = 0.f;
          }
        }
      }
      else
      {
        for (std::size_t j = 0; j < d; j++)
        {
          input_n[i][j] = 0.f;
        }
      }
    }

    for (std::size_t i = 0; i < n_out; i++)
    {
      output_n[i] = outputs_ + i * d;
      for (std::size_t j = 0; j < d; j++)
      {
        output_n[i][j] = 0.f;
      }
    }

    dsp.compute(d, input_n, output_n);

    audio_out.samples.resize(n_out);
    for (std::size_t i = 0; i < n_out; i++)
    {
      audio_out.samples[i].resize(d);
      for (std::size_t j = 0; j < d; j++)
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
}
}
