#define CATCH_CONFIG_MAIN
#define DR_WAV_IMPLEMENTATION 1
#include <catch.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/nodes/sound_ref.hpp>
#include <ossia/dataflow/nodes/sound_mmap.hpp>

TEST_CASE ("test_sound_ref", "test_sound_ref")
{
  using namespace ossia;
  nodes::sound_ref snd;
  snd.set_sound(std::vector<float_vector>{ {0.1, 0.2, 0.3, 0.4} });

  execution_state e;
  e.bufferSize = 9;

  snd.requested_tokens.push_back(simple_token_request{0_tv, 0_tv, 0_tv});
  snd.requested_tokens.push_back(simple_token_request{0_tv, 4_tv, 0_tv});
  snd.requested_tokens.push_back(simple_token_request{0_tv, 0_tv, 4_tv});
  snd.requested_tokens.push_back(simple_token_request{0_tv, 4_tv, 4_tv});
  snd.requested_tokens.push_back(simple_token_request{0_tv, 0_tv, 8_tv});
  snd.requested_tokens.push_back(simple_token_request{0_tv, 1_tv, 8_tv});

  for(auto tk : snd.requested_tokens)
  {
    snd.run(tk, {e});
  }

  auto op = snd.root_outputs()[0]->target<audio_port>()->samples;
  audio_vector expected{audio_channel{0.1f, 0.2f, 0.3f, 0.4f, 0.1f, 0.2f, 0.3f, 0.4f, 0.1f}};
  for(int i = 0; i < 9; i++)
  {
    REQUIRE(expected[0][i] - op[0][i] < 0.00001);
  }
  REQUIRE(op == expected);
}

#if defined(__GNUC__) || defined (__clang__)
// http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
// https://gist.github.com/Jon-Schneider/8b7c53d27a7a13346a643dac9c19d34f
struct __attribute__((packed, aligned(8))) test_wave  {
  static constexpr int nc = 1;
  static constexpr int F = 44100;
  static constexpr int ns = 1;
  static constexpr int M = 4;

    // RIFF Header
    char ckID[4] = {'R', 'I', 'F', 'F'};
    int cksize =  4 + 48 + 12 + (8 + M*nc*ns + 0);
    char WAVEID[4] = {'W', 'A', 'V', 'E'};

    // Format Header
    char fmt[4] = { 'f', 'm', 't', ' ' };
    int fmt_cksize = 18;
    short wFormatTag = 3;
    short nChannels = nc;
    int nSamplesPerSec = F;
    int nAvgBytesPerSec = F * M * nc;
    short nBlockAlign = M * nc;
    short wBitsPerSample = 8 * M;
    short cbSize = 0;

    // Facts
    char factID[4] = {'f', 'a', 'c', 't'};
    int factSize = 4;
    int dwSampleLength = nc * ns;

    // Data
    char data_header[4] = { 'd', 'a', 't', 'a' };
    int data_bytes = 4 * 1 * 32;

    float v[M*nc*ns] = { 0.1, 0.2, 0.3, 0.4 };
};

TEST_CASE ("test_sound_mmap", "test_sound_mmap")
{
  using namespace ossia;
  nodes::sound_mmap snd;

  test_wave w;
  drwav_handle h{&w, sizeof(test_wave)};
  float v[4]{};
  h.read_pcm_frames_f32(4, v);
  REQUIRE(v[0] == 0.1f);
  REQUIRE(v[1] == 0.2f);
  REQUIRE(v[2] == 0.3f);
  REQUIRE(v[3] == 0.4f);

  snd.set_sound(h);

  execution_state e;
  e.bufferSize = 9;

  snd.requested_tokens.push_back(simple_token_request{0_tv, 0_tv, 0_tv});
  snd.requested_tokens.push_back(simple_token_request{0_tv, 4_tv, 0_tv});
  snd.requested_tokens.push_back(simple_token_request{0_tv, 0_tv, 4_tv});
  snd.requested_tokens.push_back(simple_token_request{0_tv, 4_tv, 4_tv});
  snd.requested_tokens.push_back(simple_token_request{0_tv, 0_tv, 8_tv});
  snd.requested_tokens.push_back(simple_token_request{0_tv, 1_tv, 8_tv});

  for(auto tk : snd.requested_tokens)
  {
    snd.run(tk, {e});
    auto op = snd.root_outputs()[0]->target<audio_port>()->samples;
  }

  auto op = snd.root_outputs()[0]->target<audio_port>()->samples;
  audio_vector expected{audio_channel{0.1f, 0.2f, 0.3f, 0.4f, 0.1f, 0.2f, 0.3f, 0.4f, 0.1f}};
  for(int i = 0; i < 9; i++)
  {
    REQUIRE(expected[0][i] - op[0][i] < 0.00001);
  }
  REQUIRE(op == expected);
}
#endif
