#pragma once
#include <dr_wav.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <vector>

namespace ossia
{
//! What a host needs to play a file back as a loop in time: the ACID chunk
//! (tempo, meter, number of beats, loop or one shot) that Live, Acid, Reaper
//! and score read, and a sampler loop over the whole file.
struct wav_loop_info
{
  //! Beats per minute of the material; zero writes no tempo.
  double tempo{};
  //! Length in beats of the meter's denominator.
  int beats{};
  int meter_numerator{4};
  int meter_denominator{4};
  //! A one shot is played once; a loop repeats.
  bool loop{true};
};

//! Writes `frames` frames of 32-bit float audio, one pointer per channel, to
//! a WAV file with the loop metadata. Runs where it is called: from a worker,
//! never from the audio thread. Returns false when the file could not be
//! written in full.
inline bool write_loop_wav(
    std::string_view path_utf8, int channels, int rate, int64_t frames,
    const float* const* data, const wav_loop_info& info)
{
  if(channels <= 0 || rate <= 0 || frames < 0 || path_utf8.empty())
    return false;

  std::ofstream file{
      std::filesystem::path{std::u8string_view{
          reinterpret_cast<const char8_t*>(path_utf8.data()), path_utf8.size()}},
      std::ios::binary | std::ios::trunc};
  if(!file)
    return false;

  drwav_data_format format{};
  format.container = drwav_container_riff;
  format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
  format.channels = channels;
  format.sampleRate = rate;
  format.bitsPerSample = 32;

  drwav_metadata metadata[2]{};
  drwav_uint32 metadata_count = 0;

  drwav_smpl_loop sampler_loop{};
  if(info.loop && frames > 0)
  {
    sampler_loop.type = drwav_smpl_loop_type_forward;
    sampler_loop.firstSampleOffset = 0;
    sampler_loop.lastSampleOffset = drwav_uint32(frames - 1);

    auto& smpl = metadata[metadata_count++];
    smpl.type = drwav_metadata_type_smpl;
    smpl.data.smpl.samplePeriodNanoseconds = drwav_uint32(1e9 / rate);
    smpl.data.smpl.midiUnityNote = 60;
    smpl.data.smpl.sampleLoopCount = 1;
    smpl.data.smpl.pLoops = &sampler_loop;
  }

  if(info.tempo > 0.)
  {
    auto& acid = metadata[metadata_count++];
    acid.type = drwav_metadata_type_acid;
    acid.data.acid.flags = info.loop ? drwav_acid_flag_stretch : drwav_acid_flag_one_shot;
    acid.data.acid.midiUnityNote = 60;
    acid.data.acid.reserved1 = 0x80;
    acid.data.acid.numBeats = drwav_uint32(info.beats > 0 ? info.beats : 0);
    acid.data.acid.meterNumerator = drwav_uint16(info.meter_numerator);
    acid.data.acid.meterDenominator = drwav_uint16(info.meter_denominator);
    acid.data.acid.tempo = float(info.tempo);
  }

  auto on_write = [](void* user, const void* src, size_t bytes) -> size_t {
    auto& f = *static_cast<std::ofstream*>(user);
    f.write(static_cast<const char*>(src), std::streamsize(bytes));
    return f ? bytes : 0;
  };
  auto on_seek = [](void* user, int offset, drwav_seek_origin origin) -> drwav_bool32 {
    auto& f = *static_cast<std::ofstream*>(user);
    switch(origin)
    {
      case DRWAV_SEEK_SET:
        f.seekp(offset, std::ios::beg);
        break;
      case DRWAV_SEEK_CUR:
        f.seekp(offset, std::ios::cur);
        break;
      default:
        f.seekp(offset, std::ios::end);
        break;
    }
    return bool(f);
  };

  drwav wav{};
  if(!drwav_init_write_with_metadata(
         &wav, &format, on_write, on_seek, &file, nullptr,
         metadata_count > 0 ? metadata : nullptr, metadata_count))
    return false;

  constexpr int64_t chunk = 4096;
  std::vector<float> interleaved(std::size_t(chunk * channels));
  int64_t written = 0;
  while(written < frames)
  {
    const int64_t n = std::min(chunk, frames - written);
    for(int64_t i = 0; i < n; i++)
      for(int c = 0; c < channels; c++)
        interleaved[std::size_t(i * channels + c)] = data[c][written + i];
    if(drwav_write_pcm_frames(&wav, drwav_uint64(n), interleaved.data()) != drwav_uint64(n))
      break;
    written += n;
  }

  drwav_uninit(&wav);
  file.flush();
  return written == frames && bool(file);
}
}
