#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/token_request.hpp>
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/nodes/media.hpp>
#include <samplerate.h>
#include <boost/circular_buffer.hpp>
namespace ossia
{
struct repitch_stretcher
{
  struct resample_channel
  {
    resample_channel(int buffersize)
      : resampler{src_new(SRC_SINC_BEST_QUALITY, 1, nullptr)}
      , next_sample_to_read{0}
      , data(10 * buffersize)
    {

    }
    resample_channel(resample_channel&& other)
      : resampler{other.resampler}
      , next_sample_to_read{other.next_sample_to_read}
      , data{std::move(other.data)}
    {
      other.resampler = nullptr;
    }
    resample_channel(const resample_channel&) = delete;
    resample_channel& operator=(const resample_channel&) = delete;
    resample_channel& operator=(resample_channel&&) = delete;

    ~resample_channel()
    {
      if(resampler)
        src_delete(resampler);
    }

    SRC_STATE* resampler;
    int64_t next_sample_to_read;
    boost::circular_buffer<float> data;
  };

  repitch_stretcher(int channels, int bufferSize)
  {
    repitchers.reserve(channels);
    while(repitchers.size() < channels)
    {
      repitchers.emplace_back(bufferSize);
    }
  }

  std::vector<resample_channel> repitchers;

  void run(
      ossia::audio_span& data,
      const ossia::token_request& t,
      ossia::exec_state_facade e,
      const std::size_t chan,
      const int64_t len,
      const int64_t samples_to_read,
      const int64_t samples_to_write,
      ossia::audio_port& ap) noexcept
  {
    auto input = (float*) alloca(sizeof(float) * samples_to_read);
    auto output = (float*) alloca(sizeof(float) * samples_to_write);

    for (std::size_t i = 0; i < chan; ++i)
    {
      auto& source_channel = data[i];

      for(int64_t j = 0; j < samples_to_read; j++)
        input[j] = float(source_channel[j + t.prev_date]);

      while(int64_t(repitchers[i].data.size()) < samples_to_write)
      {
        const int64_t n = repitchers[i].next_sample_to_read;

        SRC_DATA data;
        data.data_in = input;
        data.data_out = output;
        data.input_frames = samples_to_read;
        data.output_frames = samples_to_write;
        data.input_frames_used = 0;
        data.output_frames_gen = 0;
        data.src_ratio = 1. / t.speed;

        // Copy samples
        if(n + samples_to_read < len)
        {
          for(int j = 0; j < samples_to_read; j++)
            input[j] = float(source_channel[j + n]);

          data.end_of_input = 0;
        }
        else
        {
          const int64_t max_samples_to_read = len - n;
          for(int64_t j = 0; j < max_samples_to_read; j++)
            input[j] = float(source_channel[j + n]);
          for(int64_t j = max_samples_to_read; j < samples_to_read; j++)
            input[j] = 0.f;
          data.end_of_input = 1;
        }

        // Resample
        src_process(repitchers[i].resampler, &data);

        // Put output in circular buffer
        for(int j = 0; j < data.output_frames_gen; j++)
          repitchers[i].data.push_back(output[j]);

        repitchers[i].next_sample_to_read += data.input_frames_used;
      }

      auto it = repitchers[i].data.begin();
      for(int j = 0; j < samples_to_write; j++)
      {
        ap.samples[i][j + t.offset] = double(*it);
        ++it;
      }

      repitchers[i].data.erase_begin(samples_to_write);
    }
  }
};
}
