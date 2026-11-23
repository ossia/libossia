// The libav streaming sound node (what score streams large files and video
// soundtracks with) on compressed files: FLAC, MP3, AAC, Opus, Vorbis,
// encoded here with libav from a chirp, so that a stretch of signal names
// where in the file it comes from.
//
// Frame 0 of the node is frame 0 of the signal that was encoded: whatever the
// codec adds in front of it (encoder delay, priming, pre-skip) is not part of
// the content, and a read anywhere, after any seek, lands on the signal with
// no silence of its own.

#include "include_catch.hpp"

#include <ossia/dataflow/nodes/sound_libav.hpp>
#include <ossia/detail/libav.hpp>
#include <ossia/detail/thread.hpp>

#include <cmath>

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <random>
#include <string>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/log.h>
}

namespace
{
constexpr double seconds = 3.;

struct Codec
{
  const char* encoder;
  const char* muxer;
  const char* extension;
  int rate;
  bool lossless;
};

const Codec codecs[] = {
    {"flac", "flac", "flac", 44100, true},
    {"libmp3lame", "mp3", "mp3", 44100, false},
    {"aac", "mp4", "m4a", 44100, false},
    {"libopus", "ogg", "opus", 48000, false},
    {"libvorbis", "ogg", "ogg", 44100, false},
};

//! A rising chirp: every stretch of it is unlike every other.
std::vector<float> chirp(int rate)
{
  const int n = int(seconds * rate);
  std::vector<float> s(n);
  const double f0 = 200., f1 = 3000., T = seconds;
  for(int i = 0; i < n; i++)
  {
    const double t = double(i) / rate;
    const double phase = 2. * M_PI * (f0 * t + (f1 - f0) * t * t / (2. * T));
    s[i] = float(0.5 * std::sin(phase));
  }
  return s;
}

void put_sample(AVFrame* f, AVSampleFormat fmt, int i, float v)
{
  switch(fmt)
  {
    case AV_SAMPLE_FMT_FLT:
    case AV_SAMPLE_FMT_FLTP:
      reinterpret_cast<float*>(f->data[0])[i] = v;
      break;
    case AV_SAMPLE_FMT_S16:
    case AV_SAMPLE_FMT_S16P:
      reinterpret_cast<int16_t*>(f->data[0])[i] = int16_t(std::lround(v * 32767.));
      break;
    case AV_SAMPLE_FMT_S32:
    case AV_SAMPLE_FMT_S32P:
      reinterpret_cast<int32_t*>(f->data[0])[i]
          = int32_t(std::llround(double(v) * 2147483647.));
      break;
    case AV_SAMPLE_FMT_DBL:
    case AV_SAMPLE_FMT_DBLP:
      reinterpret_cast<double*>(f->data[0])[i] = v;
      break;
    default:
      break;
  }
}

//! Encodes a mono signal; an empty string when it worked, else why not.
std::string
encode(const std::string& path, const Codec& codec, const std::vector<float>& sig)
{
  const AVCodec* enc = avcodec_find_encoder_by_name(codec.encoder);
  if(!enc)
    return std::string{"no encoder "} + codec.encoder;

  AVFormatContext* fmt{};
  if(avformat_alloc_output_context2(&fmt, nullptr, codec.muxer, path.c_str()) < 0
     || !fmt)
    return "no muxer";
  AVStream* st = avformat_new_stream(fmt, nullptr);
  AVCodecContext* c = avcodec_alloc_context3(enc);
  c->sample_rate = codec.rate;
  av_channel_layout_default(&c->ch_layout, 1);
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(61, 13, 100)
  const void* cfg{};
  int nfmt{};
  avcodec_get_supported_config(c, enc, AV_CODEC_CONFIG_SAMPLE_FORMAT, 0, &cfg, &nfmt);
  const auto* fmts = static_cast<const AVSampleFormat*>(cfg);
  c->sample_fmt = (fmts && nfmt > 0) ? fmts[0] : AV_SAMPLE_FMT_FLTP;
#else
  c->sample_fmt = enc->sample_fmts ? enc->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
#endif
  if(!codec.lossless)
    c->bit_rate = 160000;
  c->time_base = AVRational{1, codec.rate};
  c->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
  if(fmt->oformat->flags & AVFMT_GLOBALHEADER)
    c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  if(avcodec_open2(c, enc, nullptr) < 0)
  {
    avcodec_free_context(&c);
    avformat_free_context(fmt);
    return "could not open the encoder";
  }
  avcodec_parameters_from_context(st->codecpar, c);
  st->time_base = c->time_base;
  if(avio_open(&fmt->pb, path.c_str(), AVIO_FLAG_WRITE) < 0
     || avformat_write_header(fmt, nullptr) < 0)
  {
    avcodec_free_context(&c);
    avformat_free_context(fmt);
    return "could not write the header";
  }

  AVPacket* pkt = av_packet_alloc();
  auto drain = [&] {
    while(avcodec_receive_packet(c, pkt) == 0)
    {
      av_packet_rescale_ts(pkt, c->time_base, st->time_base);
      pkt->stream_index = st->index;
      av_interleaved_write_frame(fmt, pkt);
    }
  };

  const bool variable
      = (enc->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE) || c->frame_size <= 0;
  const int frame_size = c->frame_size > 0 ? c->frame_size : 1024;
  const bool small_last = enc->capabilities & AV_CODEC_CAP_SMALL_LAST_FRAME;
  AVFrame* f = av_frame_alloc();
  f->format = c->sample_fmt;
  av_channel_layout_copy(&f->ch_layout, &c->ch_layout);
  f->sample_rate = codec.rate;
  f->nb_samples = frame_size;
  av_frame_get_buffer(f, 0);

  const int total = int(sig.size());
  for(int pos = 0; pos < total; pos += frame_size)
  {
    const int n = std::min(frame_size, total - pos);
    av_frame_make_writable(f);
    f->nb_samples = (n < frame_size && !(variable || small_last)) ? frame_size : n;
    for(int i = 0; i < f->nb_samples; i++)
      put_sample(f, c->sample_fmt, i, i < n ? sig[pos + i] : 0.f);
    f->pts = pos;
    avcodec_send_frame(c, f);
    drain();
  }
  avcodec_send_frame(c, nullptr);
  drain();
  av_write_trailer(fmt);
  avio_closep(&fmt->pb);
  av_frame_free(&f);
  av_packet_free(&pkt);
  avcodec_free_context(&c);
  avformat_free_context(fmt);
  return {};
}

struct TempDir
{
  std::filesystem::path path;
  TempDir()
  {
    std::random_device rd;
    path = std::filesystem::temp_directory_path()
           / ("ossia-libav-codecs-" + std::to_string(rd()));
    std::filesystem::create_directories(path);
  }
  ~TempDir()
  {
    std::error_code ec;
    std::filesystem::remove_all(path, ec);
  }
};

//! A node on the file, as a Sound process would have it.
struct Reader
{
  ossia::nodes::sound_libav node;
  Reader(const std::string& path, int rate)
  {
    ossia::libav_handle h;
    h.open(path, 0, rate);
    REQUIRE(bool(h));
    node.set_sound(std::move(h));
  }
  std::vector<float> read(int64_t start, int64_t n)
  {
    std::vector<float> buf(n, 12345.f);
    float* out[1] = {buf.data()};
    node.fetch_audio(start, n, out);
    return buf;
  }
};

//! The shift that best aligns b on a (b[i + lag] ~ a[i]), and how well.
std::pair<int, double>
best_lag(const std::vector<float>& a, const std::vector<float>& b, int max_lag)
{
  int best = 0;
  double best_c = -2.;
  const int n = int(std::min(a.size(), b.size())) - 2 * max_lag;
  for(int lag = -max_lag; lag <= max_lag; lag++)
  {
    double ab = 0, aa = 0, bb = 0;
    for(int i = max_lag; i < max_lag + n; i++)
    {
      const double x = a[i], y = b[i + lag];
      ab += x * y;
      aa += x * x;
      bb += y * y;
    }
    const double c = (aa > 0 && bb > 0) ? ab / std::sqrt(aa * bb) : 0.;
    if(c > best_c)
    {
      best_c = c;
      best = lag;
    }
  }
  return {best, best_c};
}

//! The longest run of samples that are (close to) silent, and where it starts.
std::pair<int, int> longest_silence(const std::vector<float>& v)
{
  int best = 0, best_at = -1, run = 0;
  for(int i = 0; i < int(v.size()); i++)
  {
    if(std::abs(v[i]) < 1e-3f)
    {
      if(++run > best)
      {
        best = run;
        best_at = i - run + 1;
      }
    }
    else
    {
      run = 0;
    }
  }
  return {best, best_at};
}

//! Checks that `got` is the signal from `at` on.
void check_lands(
    const Codec& codec, const std::vector<float>& sig, int64_t at,
    const std::vector<float>& got)
{
  INFO("read at " << at << " of " << got.size() << " frames");
  REQUIRE(at + int64_t(got.size()) <= int64_t(sig.size()));
  const std::vector<float> want(sig.begin() + at, sig.begin() + at + got.size());

  const auto [lag, corr] = best_lag(want, got, 1500);
  const auto [silence, silence_at] = longest_silence(got);
  INFO(
      "lag " << lag << ", correlation " << corr << ", longest silence " << silence
             << " at " << silence_at);
  CHECK(lag == 0);
  CHECK(corr > 0.95);
  // A sine crosses zero in one sample; anything longer is missing content.
  CHECK(silence < 8);

  if(codec.lossless)
  {
    // 16 bits: within half a step of what was encoded.
    int wrong = 0, first_wrong = -1;
    for(std::size_t i = 0; i < got.size(); i++)
      if(std::abs(got[i] - want[i]) > 1e-4f)
        if(wrong++ == 0)
          first_wrong = int(i);
    INFO("first differing sample " << first_wrong);
    CHECK(wrong == 0);
  }
}
}

TEST_CASE(
    "A read of the libav streaming node lands where the signal is, whatever the "
    "codec",
    "[sound][libav][codecs]")
{
  ossia::set_thread_pinned(ossia::thread_type::Ui, 0);
  av_log_set_level(AV_LOG_ERROR);
  TempDir dir;

  int tested = 0;
  for(const auto& codec : codecs)
  {
    const auto sig = chirp(codec.rate);
    const std::string path
        = (dir.path / (std::string{"chirp."} + codec.extension)).string();
    const auto why = encode(path, codec, sig);
    INFO("codec " << codec.encoder);
    if(!why.empty())
    {
      WARN("skipped " << codec.encoder << ": " << why);
      continue;
    }
    tested++;

    constexpr int64_t N = 8192;

    // A first read, at a position
    {
      for(int64_t at :
          {int64_t(0), int64_t(1000), int64_t(20000), int64_t(30000), int64_t(70000),
           int64_t(sig.size() - N)})
      {
        Reader r{path, codec.rate};
        check_lands(codec, sig, at, r.read(at, N));
      }
    }

    // Seeks forward and backward on one node
    {
      Reader r{path, codec.rate};
      check_lands(codec, sig, 0, r.read(0, N));
      check_lands(codec, sig, 70000, r.read(70000, N));
      check_lands(codec, sig, 20000, r.read(20000, N));
      check_lands(codec, sig, 100000, r.read(100000, N));
      check_lands(codec, sig, 512, r.read(512, N));
    }

    // Small sequential reads, as playback makes them
    {
      for(int64_t from : {int64_t(0), int64_t(50000)})
      {
        Reader r{path, codec.rate};
        std::vector<float> all;
        for(int k = 0; k < 24; k++)
        {
          auto b = r.read(from + k * 512, 512);
          all.insert(all.end(), b.begin(), b.end());
        }
        check_lands(codec, sig, from, all);
      }
    }

    // At another rate than the file's, as when the graph runs at 48 kHz
    {
      const int out_rate = codec.rate == 48000 ? 44100 : 48000;
      const auto resampled = chirp(out_rate);
      Reader r{path, out_rate};
      for(int64_t at : {int64_t(0), int64_t(40000), int64_t(9000)})
      {
        INFO("at " << out_rate << " Hz");
        check_lands(
            Codec{codec.encoder, "", "", out_rate, false}, resampled, at, r.read(at, N));
      }
    }

    // The length is the signal's, in frames and not in the stream's time base
    {
      Reader r{path, codec.rate};
      const auto len = int64_t(r.node.duration());
      INFO("duration " << len << " for " << sig.size());
      CHECK(len >= int64_t(sig.size()));
      CHECK(len <= int64_t(sig.size()) + 2048);
    }
  }
  CHECK(tested > 0);
}
