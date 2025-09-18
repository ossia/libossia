#pragma once
#include <ossia/audio/audio_parameter.hpp>
#include <ossia/audio/audio_tick.hpp>
#include <ossia/detail/lockfree_queue.hpp>

#include <smallfun.hpp>

#include <atomic>

#include <ossia-config.hpp>

namespace ossia
{
class OSSIA_EXPORT audio_engine
{
public:
  explicit audio_engine();
  virtual ~audio_engine();

  virtual bool running() const = 0;
  virtual void wait(int milliseconds);
  virtual void stop();

  void gc();
  void sync();

  using fun_type = smallfun::function<void(const ossia::audio_tick_state&), 256>;
  void set_tick(fun_type&& t);
  void load_audio_tick();

  // From main thread to audio thread
  ossia::spsc_queue<fun_type> tick_funlist;

  // From audio thread to main thread
  ossia::spsc_queue<fun_type> tick_gc;

  fun_type audio_tick;

  std::atomic_bool stop_processing{};
  std::atomic_bool stop_received{};

  int effective_sample_rate{};
  int effective_buffer_size{};
  int effective_inputs{};
  int effective_outputs{};

  void tick_start() { load_audio_tick(); }
  void tick_clear() { stop_received = true; }
  void tick_end() { }

private:
  std::shared_ptr<audio_engine> self;
  std::atomic_int64_t request{-1};
  std::atomic_int64_t reply{-1};
};

OSSIA_EXPORT
ossia::audio_engine* make_audio_engine(
    std::string proto, std::string name, std::string req_in, std::string req_out,
    int& inputs, int& outputs, int& rate, int& bs);
}
