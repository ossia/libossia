#pragma once
#include <ossia/detail/config.hpp>

#if __has_include(<libavcodec/avcodec.h>) && \
    __has_include(<libavformat/avformat.h>) && \
    __has_include(<libavdevice/avdevice.h>) && \
    __has_include(<libavutil/frame.h>) && \
    __has_include(<libswresample/swresample.h>) && \
    __has_include(<libswscale/swscale.h>)

#define OSSIA_HAS_LIBAV 1
#include <ossia/detail/flicks.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/version.h>
#include <libswresample/swresample.h>
}

namespace ossia
{
// Try to find a flag that is likely not going to be taken by AVSEEK_FLAG_... ever
static constexpr const int OSSIA_LIBAV_SEEK_ROUGH = 0b00000100000000000000000000000000;
inline bool seek_to_flick(
    AVFormatContext* format, AVCodecContext* codec, AVStream* stream, int64_t flicks,
    int flags = 0)
{
  constexpr auto flicks_tb = AVRational{1, ossia::flicks_per_second<int64_t>};
  constexpr auto av_tb = AVRational{1, AV_TIME_BASE};

  const auto dts = av_rescale_q_rnd(flicks, flicks_tb, av_tb, AVRounding::AV_ROUND_DOWN);

  avio_flush(format->pb);
  avformat_flush(format);
  if(codec)
    avcodec_flush_buffers(codec);

  if(flags & OSSIA_LIBAV_SEEK_ROUGH)
  {
    // Seems to work better for seeking for thumbnails
    flags &= ~OSSIA_LIBAV_SEEK_ROUGH;
    if(av_seek_frame(format, -1, dts, flags) < 0)
      return false;
  }
  else
  {
    // Seems to work better for seeking for video & audio
    if(avformat_seek_file(format, -1, INT64_MIN, dts, INT64_MAX, flags) < 0)
      return false;
  }

  avio_flush(format->pb);
  avformat_flush(format);
  if(codec)
    avcodec_flush_buffers(codec);

  return true;
}

static inline int avstream_get_audio_channels(AVStream& stream) noexcept
{
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(57, 24, 100)
  return stream.codecpar->ch_layout.nb_channels;
#else
  return stream.codecpar->channels;
#endif
}

struct libav_handle
{
  AVFormatContext* format{};
  AVStream* stream{};
  AVCodecContext* codec{};
  SwrContext* resample{};

  libav_handle() = default;
  libav_handle(const libav_handle& other) = delete;
  libav_handle(libav_handle&& other)
  {
    format = other.format;
    other.format = nullptr;
    stream = other.stream;
    other.stream = nullptr;
    codec = other.codec;
    other.codec = nullptr;
    resample = other.resample;
    other.resample = nullptr;
  }
  libav_handle& operator=(const libav_handle& other) = delete;
  libav_handle& operator=(libav_handle&& other)
  {
    format = other.format;
    other.format = nullptr;
    stream = other.stream;
    other.stream = nullptr;
    codec = other.codec;
    other.codec = nullptr;
    resample = other.resample;
    other.resample = nullptr;
    return *this;
  }
  ~libav_handle()
  {
    if(format)
      cleanup();
  }

  void cleanup()
  {
    stream = nullptr;
    if(resample)
      swr_free(&resample);
    if(codec)
    {
      avcodec_free_context(&codec);
    }
    if(format)
    {
      avformat_close_input(&format);
      avformat_free_context(format);
      format = nullptr;
    }
  }

  void open(const std::string& path, int stream_index, int target_rate) noexcept
  {
    if(avformat_open_input(&format, path.c_str(), nullptr, nullptr) != 0)
    {
      cleanup();
      return;
    }
    if(avformat_find_stream_info(format, nullptr) < 0)
    {
      cleanup();
      return;
    }
    if(stream_index < 0 || stream_index >= int(format->nb_streams))
    {
      cleanup();
      return;
    }
    if(format->streams[stream_index]->codecpar->codec_type != AVMEDIA_TYPE_AUDIO)
    {
      cleanup();
      return;
    }

    stream = format->streams[stream_index];

    auto cdc = avcodec_find_decoder(stream->codecpar->codec_id);
    if(!cdc)
    {
      cleanup();
      return;
    }

    codec = avcodec_alloc_context3(cdc);
    if(avcodec_parameters_to_context(codec, stream->codecpar) < 0)
    {
      cleanup();
      return;
    }

    if(avcodec_open2(codec, cdc, nullptr) < 0)
    {
      cleanup();
      return;
    }

    AVSampleFormat out_sample_fmt{AV_SAMPLE_FMT_FLT};
    AVSampleFormat in_sample_fmt{(AVSampleFormat)stream->codecpar->format};
    int in_sample_rate = stream->codecpar->sample_rate;
    int out_sample_rate = target_rate == 0 ? in_sample_rate : target_rate;

#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(57, 24, 100)
    AVChannelLayout out_layout{stream->codecpar->ch_layout};
    AVChannelLayout in_layout{stream->codecpar->ch_layout};

    swr_alloc_set_opts2(
        &resample, &out_layout, out_sample_fmt, out_sample_rate, &in_layout,
        in_sample_fmt, in_sample_rate, 0, nullptr);
#else
    int64_t out_layout = stream->codecpar->channel_layout;
    int64_t in_layout = stream->codecpar->channel_layout;

    resample = swr_alloc_set_opts(
        resample, out_layout, out_sample_fmt, out_sample_rate, in_layout, in_sample_fmt,
        in_sample_rate, 0, nullptr);
#endif
    if(resample)
      swr_init(resample);
  }

  int rate() const noexcept { return stream->codecpar->sample_rate; }
  int channels() const noexcept { return avstream_get_audio_channels(*stream); }

  int64_t totalPCMFrameCount() const noexcept
  {
    if(stream->duration > 0)
    {
      return stream->duration;
    }
    else if(format->duration > 0)
    {
      double seconds = format->duration / double(AV_TIME_BASE);
      double frames = seconds * stream->codecpar->sample_rate;
      return frames;
    }
    else if(stream->nb_frames > 0)
    {
      return av_rescale_q(
          stream->nb_frames, stream->r_frame_rate,
          AVRational{1, stream->codecpar->sample_rate});
    }
    else
    {
      return 0;
    }
  }

  operator bool() const noexcept { return bool(format); }

  void fetch(int64_t frame, int samples_to_write, auto func)
  {
    // First seek
    ossia::seek_to_flick(
        format, codec, stream,
        ossia::flicks_per_second<double> * frame / stream->codecpar->sample_rate,
        AVSEEK_FLAG_ANY);

    const std::size_t channels = this->channels();
    std::vector<float> tmp;
    if(channels == 0)
      return;

    int processed = 0;
    while(processed < samples_to_write)
    {
      // Need to fetch more data
      {
        auto packet = av_packet_alloc();
        int ret{};
        {
          ret = av_read_frame(format, packet);

          while(ret >= 0 && ret != AVERROR(EOF) && packet->stream_index != stream->index)
          {
            av_packet_unref(packet);
            ret = av_read_frame(format, packet);
          }
          if(ret == AVERROR(EOF))
          {
            break;
          }
        }
        if(ret < 0)
        {
          return;
        }

        ret = avcodec_send_packet(codec, packet);
        if(ret == 0)
        {
          auto avframe = av_frame_alloc();
          ret = avcodec_receive_frame(codec, avframe);
          if(ret == 0)
          {
            const int av_frame_start = avframe->best_effort_timestamp;
            const int samples = avframe->nb_samples;

            // It's possible that we get a frame that is just after what we have asked, thus
            // frame - av_frame_start can sometimes be negative
            const int offset = (frame < av_frame_start) ? 0 : (frame - av_frame_start);
            if(offset >= samples)
            {
              // ffmpeg didn't even manage to seek to the correct frame, we have to read another
              av_frame_free(&avframe);
              av_packet_unref(packet);
              av_packet_free(&packet);
              continue;
            }

            // The actual samples we can read here:
            //samples = samples - (frame - av_frame_start);

            tmp.resize(samples * channels);
            float* out_ptr = tmp.data();

            // Note: this function is only called with swr_convert's rate not changing
            int read_samples = swr_convert(
                resample, (uint8_t**)&out_ptr, samples,
                (const uint8_t**)avframe->extended_data, samples);

            auto end = tmp.data() + tmp.size();

            read_samples -= offset;
            if(read_samples <= 0)
            {
              av_frame_free(&avframe);
              av_packet_unref(packet);
              av_packet_free(&packet);
              return;
            }

            out_ptr += offset * channels;

            for(int i = 0; i < read_samples; i++)
            {
              func(out_ptr, end);

              processed++;
              if(processed == samples_to_write)
              {
                av_frame_free(&avframe);
                av_packet_unref(packet);
                av_packet_free(&packet);
                return;
              }

              out_ptr += channels;
            }

            // We read an entire packet: set "frame" to the beginning of next packet
            // so that we read it in full
            frame = avframe->best_effort_timestamp + avframe->nb_samples;
          }
          av_frame_free(&avframe);
        }
        av_packet_unref(packet);
        av_packet_free(&packet);
      }
    }
  }
};

}
#endif
