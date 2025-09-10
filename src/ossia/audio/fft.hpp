#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/pod_vector.hpp>

#include <cinttypes>
#include <cstddef>

#if defined(OSSIA_FFT_FFTW)
#if defined(OSSIA_FFTW_SINGLE_ONLY)
using fftw_plan = struct fftw_plan_s*;
using fftwf_plan = struct fftwf_plan_s*;
namespace ossia
{
using fft_plan = fftwf_plan;
using fft_real = float;
using fft_complex = float[2];
struct fft_temp_storage
{
};
}
#elif defined(OSSIA_FFTW_DOUBLE_ONLY)
using fftw_plan = struct fftw_plan_s*;
using fftwf_plan = struct fftwf_plan_s*;
namespace ossia
{
using fft_plan = fftw_plan;
using fft_real = double;
using fft_complex = double[2];
struct fft_temp_storage
{
};
}
#endif
#elif defined(OSSIA_FFT_KFR)
namespace ossia
{
using fft_plan = void*;
using fft_real = double;
using fft_complex = double[2];
using fft_temp_storage = ossia::pod_vector<uint8_t>;
}
#else
namespace ossia
{
using fft_plan = void*;
using fft_real = double;
using fft_complex = double[2];
struct fft_temp_storage
{
};
}
#endif

namespace ossia
{
class OSSIA_EXPORT fft
{
public:
  explicit fft(std::size_t newSize) noexcept;
  fft() noexcept
      : fft{16}
  {
  }

  ~fft();

  static constexpr double norm(std::size_t sz) noexcept { return 1. / sz; }

  void reset(std::size_t newSize);

  fft_complex* execute(float* input, std::size_t sz) noexcept;
  fft_complex* execute() noexcept;

  [[nodiscard]] fft_real* input() const noexcept { return m_input; }

private:
  fft_plan m_fw = {};
  std::size_t m_size = 0;
  std::size_t m_storage_size = 0;
  fft_real* m_input{};
  fft_complex* m_output{};
  fft_temp_storage m_storage;
};

class OSSIA_EXPORT rfft
{
public:
  explicit rfft(std::size_t newSize) noexcept;
  ~rfft();

  static constexpr double norm(std::size_t sz) noexcept { return 1. / sz; }

  void reset(std::size_t newSize);

  fft_real* execute(fft_complex* input) noexcept;
  fft_real* execute() noexcept;

  [[nodiscard]] fft_complex* input() const noexcept { return m_input; }

private:
  fft_plan m_fw = {};
  std::size_t m_size = 0;
  std::size_t m_storage_size = 0;
  fft_complex* m_input{};
  fft_real* m_output{};
  fft_temp_storage m_storage;
};
}
