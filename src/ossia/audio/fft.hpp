#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/pod_vector.hpp>

#include <array>
#include <cinttypes>
#include <complex>
#include <cstddef>
#include <memory>
#include <span>

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

  //! For a new size: a plan it had for it, or a new one (planned quickly,
  //! as this can happen on the audio thread)
  void reset(std::size_t newSize);
  //! Plans for this size and the ones an audio server most often switches
  //! to around it, ahead of time: not on the audio thread
  void prepare(std::size_t newSize);

  fft_complex* execute(float* input, std::size_t sz) noexcept;
  fft_complex* execute() noexcept;

  [[nodiscard]] fft_real* input() const noexcept { return m_input; }
  //! The size it was set up for, and the (power of two) size of input().
  [[nodiscard]] std::size_t size() const noexcept { return m_size; }
  [[nodiscard]] std::size_t storage_size() const noexcept { return m_storage_size; }

private:
  void build(std::size_t newSize, bool measure) noexcept;
  void release() noexcept;
  void use(std::size_t newSize, bool measure);

  fft_plan m_fw = {};
  std::size_t m_size = 0;
  std::size_t m_storage_size = 0;
  fft_real* m_input{};
  fft_complex* m_output{};
  fft_temp_storage m_storage;

  //! Plans for the sizes used before: an audio server can switch between a
  //! few block sizes, and planning allocates (and measures, with FFTW).
  struct spare
  {
    fft_plan fw = {};
    std::size_t size = 0;
    std::size_t storage_size = 0;
    fft_real* input{};
    fft_complex* output{};
    fft_temp_storage storage;
    std::size_t used = 0;
  };
  void swap(spare& s) noexcept;
  std::array<spare, 3> m_spares{};
  std::size_t m_uses = 0;
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

//! Exact-length transforms, including lengths that are not powers of two.
//! Construct off the audio thread. Execution does not allocate. Use one plan
//! per concurrent execution.
class OSSIA_EXPORT exact_fft
{
public:
  enum class kind { real_forward, real_inverse, complex_forward, complex_inverse };

  exact_fft(std::size_t size, kind mode);
  ~exact_fft();
  exact_fft(const exact_fft&) = delete;
  exact_fft& operator=(const exact_fft&) = delete;

  //! N real samples to N/2+1 complex bins (DC through the nonnegative half).
  void execute(std::span<const float> input, std::span<std::complex<float>> output) noexcept;
  //! N/2+1 Hermitian bins to N real samples. No implicit 1/N normalization.
  void execute(
      std::span<const std::complex<float>> input, std::span<float> output,
      float scale = 1.f) noexcept;
  //! N complex samples to N bins, or an output prefix for a forward transform.
  //! The buffers may have the same start but must not otherwise overlap.
  //! No implicit 1/N normalization.
  void execute(
      std::span<const std::complex<float>> input, std::span<std::complex<float>> output,
      float scale = 1.f) noexcept;

private:
  struct plan;
  std::unique_ptr<plan> m_plan;
};
}
