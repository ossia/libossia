#include <ossia/detail/config.hpp>

#include <ossia/audio/fft.hpp>

#include "include_catch.hpp"

#include <cmath>
#include <complex>
#include <numbers>
#include <vector>

namespace
{
// Reference: the DFT of `signal` zero-padded to `n` samples.
std::vector<std::complex<double>> padded_dft(const std::vector<float>& signal, int n)
{
  std::vector<std::complex<double>> res(n / 2 + 1);
  for(int k = 0; k <= n / 2; k++)
    for(std::size_t t = 0; t < signal.size() && t < std::size_t(n); t++)
      res[k] += double(signal[t])
                * std::polar(1., -2. * std::numbers::pi * k * double(t) / n);
  return res;
}

void check_spectrum(
    const ossia::fft_complex* out, const std::vector<std::complex<double>>& ref)
{
  for(std::size_t k = 0; k < ref.size(); k++)
  {
    INFO("bin " << k);
    CHECK(std::abs(out[k][0] - ref[k].real()) < 1e-3);
    CHECK(std::abs(out[k][1] - ref[k].imag()) < 1e-3);
  }
}

std::vector<float> ramp(std::size_t n)
{
  std::vector<float> v(n);
  for(std::size_t i = 0; i < n; i++)
    v[i] = std::sin(0.3f * i) + 0.01f * i;
  return v;
}
}

// A size that is not a power of two transforms a power-of-two storage: what
// lies past the input must read as zeros, whatever the storage held before.
TEST_CASE("fft_non_power_of_two_zero_pads", "fft")
{
  constexpr std::size_t size = 100;
  constexpr int storage = 128;
  ossia::fft fft{size};

  auto dirty = [&] {
    for(int i = 0; i < storage; i++)
      fft.input()[i] = 1000.;
  };

  SECTION("full input")
  {
    auto signal = ramp(size);
    dirty();
    check_spectrum(fft.execute(signal.data(), signal.size()), padded_dft(signal, storage));
  }

  SECTION("longer input: only the first size samples")
  {
    auto signal = ramp(size + 40);
    dirty();
    auto out = fft.execute(signal.data(), signal.size());
    signal.resize(size);
    check_spectrum(out, padded_dft(signal, storage));
  }

  SECTION("shorter input")
  {
    auto signal = ramp(37);
    dirty();
    check_spectrum(fft.execute(signal.data(), signal.size()), padded_dft(signal, storage));
  }

  SECTION("repeated calls")
  {
    auto signal = ramp(size);
    for(int i = 0; i < 3; i++)
      check_spectrum(
          fft.execute(signal.data(), signal.size()), padded_dft(signal, storage));
  }
}

TEST_CASE("fft_power_of_two_short_input", "fft")
{
  constexpr std::size_t size = 64;
  ossia::fft fft{size};
  for(std::size_t i = 0; i < size; i++)
    fft.input()[i] = -5.;

  auto signal = ramp(20);
  check_spectrum(fft.execute(signal.data(), signal.size()), padded_dft(signal, size));
}

TEST_CASE("exact FFT preserves bin spacing and inverse scaling", "fft")
{
  using kind = ossia::exact_fft::kind;
  using sample = std::complex<float>;
  for(std::size_t n : {1, 2, 17, 64, 100, 480})
  {
    CAPTURE(n);
    auto real = ramp(n);
    std::vector<sample> half(n / 2 + 1);
    ossia::exact_fft real_forward{n, kind::real_forward};
    real_forward.execute(real, half);
    const auto expected_real = padded_dft(real, int(n));
    for(std::size_t k = 0; k < half.size(); ++k)
      CHECK(std::abs(std::complex<double>{half[k]} - expected_real[k]) < 1e-3);

    std::vector<float> roundtrip(n);
    ossia::exact_fft real_inverse{n, kind::real_inverse};
    real_inverse.execute(half, roundtrip, 1.f / n);
    for(std::size_t i = 0; i < n; ++i)
      CHECK(std::abs(roundtrip[i] - real[i]) < 1e-4);

    std::vector<sample> signal(n), transformed(n);
    constexpr std::size_t bin = 3;
    for(std::size_t i = 0; i < n; ++i)
      signal[i] = std::polar(1.f, float(-2. * std::numbers::pi * bin * i / n));
    ossia::exact_fft complex_forward{n, kind::complex_forward};
    complex_forward.execute(signal, transformed);
    for(std::size_t k = 0; k < n; ++k)
    {
      const auto expected = k == (n - bin % n) % n ? sample{float(n), 0.f} : sample{};
      CHECK(std::abs(transformed[k] - expected) < 1e-3);
    }

    ossia::exact_fft complex_inverse{n, kind::complex_inverse};
    complex_inverse.execute(transformed, transformed, 1.f / n);
    for(std::size_t i = 0; i < n; ++i)
      CHECK(std::abs(transformed[i] - signal[i]) < 1e-4);

    complex_forward.execute(signal, std::span<sample>{signal}.first(half.size()));
    for(std::size_t k = 0; k < half.size(); ++k)
    {
      const auto expected = k == (n - bin % n) % n ? sample{float(n), 0.f} : sample{};
      CHECK(std::abs(signal[k] - expected) < 1e-3);
    }
  }
}
