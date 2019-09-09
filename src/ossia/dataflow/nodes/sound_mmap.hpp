#pragma once
#include <ossia/audio/audio_parameter.hpp>
#include <ossia/audio/drwav_handle.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/nodes/sound.hpp>
#include <ossia/dataflow/audio_stretch_mode.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/dataflow/nodes/media.hpp>

#include <type_traits>

namespace ossia::nodes
{
// TODO refactor with AudioDecoder
template <typename SampleFormat, int N>
constexpr audio_sample convert_sample(SampleFormat i);

template <>
constexpr audio_sample convert_sample<int16_t, 16>(int16_t i)
{
    // TODO division -> multiplication
  if constexpr (std::is_same_v<ossia::audio_sample, float>)
    return (i + .5f) / (0x7FFF + .5f);
  else
    return (i + .5) / (0x7FFF + .5);
}

template <>
constexpr audio_sample convert_sample<int32_t, 24>(int32_t i)
{
  if constexpr (std::is_same_v<ossia::audio_sample, float>)
    return ((int32_t)i >> 8)
           / ((audio_sample)std::numeric_limits<int32_t>::max() / 256.f);
  else
    return ((int32_t)i >> 8)
           / ((audio_sample)std::numeric_limits<int32_t>::max() / 256.);
}

template <>
constexpr audio_sample convert_sample<int32_t, 32>(int32_t i)
{
  return i / (audio_sample)(std::numeric_limits<int32_t>::max());
}

template <>
constexpr audio_sample convert_sample<float, 32>(float i)
{
  return i;
}

class sound_mmap final : public ossia::nonowning_graph_node
{
public:
    sound_mmap()
    {
        m_outlets.push_back(&audio_out);
    }

    ~sound_mmap()
    {
    }

    void set_start(std::size_t v)
    {
        start = v;
    }
    void set_upmix(std::size_t v)
    {
        upmix = v;
    }

    void set_sound(drwav_handle hdl)
    {
        m_handle = std::move(hdl);
        if(m_handle)
        {
            switch(m_handle.translatedFormatTag())
            {
            case DR_WAVE_FORMAT_PCM:
            {
                switch(m_handle.bitsPerSample())
                {
                case 16:
                {
                    m_converter = read_s16;
                    break;
                }
                case 24:
                {
                    m_converter = read_s24;
                    break;
                }
                case 32:
                {
                    m_converter = read_s32;
                    break;
                }
                }
                break;
            }
            case DR_WAVE_FORMAT_IEEE_FLOAT:
            {
                m_converter = read_f32;
                break;
            }
            default:
                m_converter = nullptr;
                break;
            }
        }
        m_resampler.reset(0_tv, m_mode, hdl.channels(), hdl.sampleRate());
    }

    void set_native_tempo(double v)
    {
      tempo = v;
    }

    void set_stretch_mode(ossia::audio_stretch_mode mode)
    {
      if(m_mode != mode)
      {
        m_mode = mode;
        m_resampler.reset(0_tv, m_mode, channels(), m_handle.sampleRate());
      }
    }

    void reset_resampler(time_value date)
    {
      m_resampler.reset(date, m_mode, channels(), m_handle.sampleRate());
    }

    static void read_s16(ossia::audio_port& ap, const ossia::token_request& t, void* data, int64_t samples)
    {
        const auto channels = ap.samples.size();
        auto d = reinterpret_cast<int16_t*>(data);

        for (int64_t j = 0; j < samples; j++)
        {
            for (std::size_t i = 0; i < channels; i++)
            {
                ap.samples[i][j + t.offset.impl]
                        = convert_sample<int16_t, 16>(d[j * channels + i]);
            }
        }
    }


    static void read_s24(ossia::audio_port& ap, const ossia::token_request& t, void* data, int64_t samples)
    {
        const auto channels = ap.samples.size();
        const auto frame_bytes = channels * 3;

        auto bytes = reinterpret_cast<uint8_t*>(data);
        for (int64_t j = 0; j < samples; j++)
        {
            for (std::size_t i = 0; i < channels; i++)
            {
                int32_t sample = 0;
                sample += (bytes[3 * i] << 8);
                sample += (bytes[3 * i + 1] << 16);
                sample += (bytes[3 * i + 2] << 24);

                ap.samples[i][j + t.offset.impl]
                        = convert_sample<int32_t, 24>(sample);
            }
            bytes += frame_bytes;
        }
    }

    static void read_s32(ossia::audio_port& ap, const ossia::token_request& t, void* data, int64_t samples)
    {
        // TODO
    }


    static void read_f32(ossia::audio_port& ap, const ossia::token_request& t, void* data, int64_t samples)
    {
        const auto channels = ap.samples.size();
        auto d = reinterpret_cast<float*>(data);

        for (int64_t j = 0; j < samples; j++)
        {
            for (std::size_t i = 0; i < channels; i++)
            {
                ap.samples[i][j + t.offset.impl] = d[j * channels + i];
            }
        }
    }


    void
    run(ossia::token_request t, ossia::exec_state_facade e) noexcept override
    {
        if(!m_handle)
            return;

        const auto channels = m_handle.channels();
        const auto max_frames = m_handle.totalPCMFrameCount();

        ossia::audio_port& ap = *audio_out.data.target<ossia::audio_port>();
        ap.samples.resize(channels);
        const int64_t max_N = std::min(t.date.impl, (int64_t)(max_frames));
        if (max_N <= 0)
            return;

        const auto frames = max_N - t.prev_date + t.offset.impl;
        if (frames <= 0)
            return;

        if (t.date > t.prev_date)
        {
            const bool ok = m_handle.seek_to_pcm_frame(t.prev_date.impl);
            if(!ok)
                return;

            void* data = nullptr;
            const auto bytes_to_allocate = sizeof(double) * frames * channels;
            if(bytes_to_allocate < 300000)
            {
                data = alloca(bytes_to_allocate);
            }
            else
            {
                m_safetyBuffer.resize(bytes_to_allocate); // TODO pector
                data = m_safetyBuffer.data();
            }

            auto count = m_handle.read_pcm_frames(frames, data);

            for (std::size_t i = 0; i < channels; i++)
            {
                ap.samples[i].resize(frames + t.offset.impl);
            }

            m_converter(ap, t, data, count);

            for (std::size_t i = 0; i < channels; i++)
            {
                do_fade(t.start_discontinuous, t.end_discontinuous, ap.samples[i],
                        t.offset.impl, frames);
            }
        }
        else
        {
            /* TODO */
        }

        // Upmix
        if (upmix != 0)
        {
            if (upmix < channels)
            {
                //
                //    // Downmix
                //    switch(upmix)
                //    {
                //      case 1:
                //      {
                //        for(std::size_t i = 1; i < chan; i++)
                //        {
                //          if(ap.samples[0].size() < ap.samples[i].size())
                //            ap.samples[0].resize(ap.samples[i].size());
                //
                //          for(std::size_t j = 0; j < ap.samples[i].size(); j++)
                //            ap.samples[0][j] += ap.samples[i][j];
                //        }
                //      }
                //      default:
                //        // TODO
                //        break;
                //    }
                //
            }
            else if (upmix > channels)
            {
                switch (channels)
                {
                case 1:
                {
                    ap.samples.resize(upmix);
                    for (std::size_t i = 1; i < upmix; i++)
                    {
                        ap.samples[i] = ap.samples[0];
                    }
                    break;
                }
                default:
                    // TODO
                    break;
                }
            }
        }

        // Move channels
        if (start != 0)
        {
            ap.samples.insert(ap.samples.begin(), start, ossia::audio_channel{});
        }
    }
    std::size_t channels() const
    {
        return m_handle ? m_handle.channels() : 0;
    }
    std::size_t duration() const
    {
        return m_handle ? m_handle.totalPCMFrameCount(): 0;
    }

private:
    std::size_t start{};
    std::size_t upmix{};
    double tempo{};
    ossia::outlet audio_out{ossia::audio_port{}};

    drwav_handle m_handle{};
    using read_fn_t = void(*)(ossia::audio_port& ap, const ossia::token_request& t, void* data, int64_t samples);
    read_fn_t m_converter{};
    std::vector<char> m_safetyBuffer;

    resampler m_resampler;
    audio_stretch_mode m_mode{};
};

}
