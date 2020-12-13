#pragma once
#include <cinttypes>
#include <cstddef>
#include <ossia_export.h>

using fftw_plan = struct fftw_plan_s*;
using fftwf_plan = struct fftwf_plan_s*;
namespace ossia
{
#define FFTW_DOUBLE_ONLY
class OSSIA_EXPORT fft
{
public:
#if defined(FFTW_SINGLE_ONLY)
  using fft_plan = fftwf_plan;
  using fft_real = float;
  using fft_complex = float[2];
#elif defined(FFTW_DOUBLE_ONLY)
  using fft_plan = fftw_plan;
  using fft_real = double;
  using fft_complex = double[2];
#endif

  explicit fft(std::size_t newSize) noexcept;
  ~fft();

  static constexpr double norm(std::size_t sz) noexcept {
    return 1. / sz;
  }

  void reset(std::size_t newSize);

  fft_complex* execute(float* input, std::size_t sz) noexcept;

private:
  fft_plan m_fw = {};
  std::size_t m_size = 0;
  fft_real* m_input{};
  fft_complex* m_output{};
};

class OSSIA_EXPORT rfft
{
public:
#if defined(FFTW_SINGLE_ONLY)
  using fft_plan = fftwf_plan;
  using fft_real = float;
  using fft_complex = float[2];
#elif defined(FFTW_DOUBLE_ONLY)
  using fft_plan = fftw_plan;
  using fft_real = double;
  using fft_complex = double[2];
#endif

  explicit rfft(std::size_t newSize) noexcept;
  ~rfft();

  static constexpr double norm(std::size_t sz) noexcept {
    return 1. / sz;
  }

  void reset(std::size_t newSize);

  fft_real* execute(fft_complex* input) noexcept;

private:
  fft_plan m_fw = {};
  std::size_t m_size = 0;
  fft_complex* m_input{};
  fft_real* m_output{};
};
}
