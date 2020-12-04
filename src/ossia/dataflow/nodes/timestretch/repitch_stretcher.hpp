#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/token_request.hpp>
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/nodes/media.hpp>
#include <cinttypes>
#include <samplerate.h>
#include <boost/circular_buffer.hpp>
namespace ossia
{
struct repitch_stretcher
{
  struct resample_channel
  {
    resample_channel(int buffersize) noexcept
      : resampler{src_new(SRC_SINC_BEST_QUALITY, 1, nullptr)}
      , data(10 * buffersize)
    {

    }
    resample_channel(resample_channel&& other) noexcept
      : resampler{other.resampler}
      , data{std::move(other.data)}
    {
      other.resampler = nullptr;
    }
    resample_channel& operator=(resample_channel&& other) noexcept
    {
      resampler = other.resampler;
      data = std::move(other.data);
      other.resampler = nullptr;
      return *this;
    }

    resample_channel(const resample_channel&) = delete;
    resample_channel& operator=(const resample_channel&) = delete;

    ~resample_channel()
    {
      if(resampler)
        src_delete(resampler);
    }

    std::vector<float> input_buffer;
    SRC_STATE* resampler;
    boost::circular_buffer<float> data;
  };

  repitch_stretcher(int channels, int bufferSize, int64_t pos)
    : next_sample_to_read{pos}
  {
    repitchers.reserve(channels);
    while(int(repitchers.size()) < channels)
    {
      repitchers.emplace_back(bufferSize);
    }
  }

  std::vector<float*> input_channels;
  std::vector<float> output_buffer;
  std::vector<resample_channel> repitchers;
  int64_t next_sample_to_read{};

  template<typename T>
  void run(
      T& audio_fetcher,
      const ossia::token_request& t,
      ossia::exec_state_facade e,
      double tempo_ratio,
      const std::size_t chan,
      const int64_t len,
      int64_t samples_to_read,
      const int64_t samples_to_write,
      const int64_t samples_offset,
      ossia::audio_port& ap) noexcept
  {
    assert(chan > 0);

    input_channels.resize(chan);
    for(std::size_t i = 0; i < chan; i++)
    {
      repitchers[i].input_buffer.resize(std::max((int64_t)16, samples_to_read));
      input_channels[i] = repitchers[i].input_buffer.data();
    }
    output_buffer.resize(samples_to_write);
    auto output = output_buffer.data();

    int64_t num_samples_available = repitchers[0].data.size();

    while(num_samples_available < samples_to_write)
    {
      audio_fetcher.fetch_audio(next_sample_to_read, samples_to_read, input_channels.data());

      SRC_DATA data;
      for (std::size_t i = 0; i < chan; ++i)
      {
        data.data_in = repitchers[i].input_buffer.data();
        data.data_out = output;
        data.input_frames = samples_to_read;
        data.output_frames = samples_to_write - num_samples_available;
        data.input_frames_used = 0;
        data.output_frames_gen = 0;
        data.src_ratio = tempo_ratio;
        data.end_of_input = 0;

        // Resample
        src_process(repitchers[i].resampler, &data);

        // Put output in circular buffer
        /*
        if(data.output_frames_gen == 0)
        {
          std::cerr << "we did not write anything new "
                    << data.input_frames  << " "
                    << data.output_frames << " "
                    << data.input_frames_used  << " "
                    << data.output_frames_gen << " "
                    << num_samples_written << std::endl;
        }
        */
        for(int j = 0; j < data.output_frames_gen; j++)
          repitchers[i].data.push_back(output[j]);
      }
      next_sample_to_read += data.input_frames_used;
      samples_to_read = 16;
      num_samples_available = repitchers[0].data.size();
    }

    for(std::size_t i = 0; i < chan; ++i)
    {
      auto it = repitchers[i].data.begin();
      for(int j = 0; j < samples_to_write; j++)
      {
        ap.samples[i][j + samples_offset] = double(*it);
        ++it;
      }

      repitchers[i].data.erase_begin(samples_to_write);
    }
  }
};
}
