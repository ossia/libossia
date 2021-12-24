#include <ossia/audio/fft.hpp>
#include <new>
#include <algorithm>

#if defined(OSSIA_FFT_FFTW)
#include <fftw3.h>
namespace ossia
{
namespace
{
#if defined(OSSIA_FFTW_SINGLE_ONLY)
  static const constexpr auto alloc_real = ::fftwf_alloc_real;
  static const constexpr auto alloc_complex = ::fftwf_alloc_complex;
  static const constexpr auto fft_free = ::fftwf_free;
  static const constexpr auto create_plan_r2c = ::fftwf_plan_dft_r2c_1d;
  static const constexpr auto run_plan_r2c = ::fftwf_execute_dft_r2c;
  static const constexpr auto create_plan_c2r = ::fftwf_plan_dft_c2r_1d;
  static const constexpr auto run_plan_c2r = ::fftwf_execute_dft_c2r;
  static const constexpr auto destroy_plan = ::fftwf_destroy_plan;
  static const constexpr auto cleanup = ::fftwf_cleanup;
  static const constexpr auto alignment_of = ::fftwf_alignment_of;

#elif defined(OSSIA_FFTW_DOUBLE_ONLY)
  static const constexpr auto alloc_real = ::fftw_alloc_real;
  static const constexpr auto alloc_complex = ::fftw_alloc_complex;
  static const constexpr auto fft_free = ::fftw_free;
  static const constexpr auto create_plan_r2c = ::fftw_plan_dft_r2c_1d;
  static const constexpr auto run_plan_r2c = ::fftw_execute_dft_r2c;
  static const constexpr auto create_plan_c2r = ::fftw_plan_dft_c2r_1d;
  static const constexpr auto run_plan_c2r = ::fftw_execute_dft_c2r;
  static const constexpr auto destroy_plan = ::fftw_destroy_plan;
  // static const constexpr auto cleanup = ::fftw_cleanup;
  // static const constexpr auto alignment_of = ::fftw_alignment_of;
#else
#define FFTW_DESTROY_INPUT 0
#define FFTW_MEASURE 0

auto alloc_real(std::size_t)
{
  return nullptr;
}
auto alloc_complex(std::size_t)
{
  return nullptr;
}
auto fft_free(void*)
{
}
auto destroy_plan(void*)
{
}
auto create_plan_r2c(std::size_t, void*, void*, int)
{
  return fft_plan{};
}
auto create_plan_c2r(std::size_t, void*, void*, int)
{
  return fft_plan{};
}
auto run_plan_r2c(fft_plan, void*, void*)
{
}
auto run_plan_c2r(fft_plan, void*, void*)
{
  return fft_plan{};
}
#endif
}

fft::fft(std::size_t newSize) noexcept
{
  m_size = newSize;
  m_input = alloc_real(newSize);
  m_output = reinterpret_cast<fft_complex*>(alloc_complex(newSize / 2 + 1));
  m_fw = create_plan_r2c(newSize, m_input, m_output, FFTW_DESTROY_INPUT | FFTW_MEASURE);
}

fft::~fft()
{
  destroy_plan(m_fw);
  fft_free(m_input);
  fft_free(m_output);
  //  cleanup();
}

void fft::reset(std::size_t newSize)
{
  this->~fft();
  new(this) fft(newSize);
}

fft_complex* fft::execute(float* input, std::size_t sz) noexcept
{
  const bool sz_ok = sz >= m_size;

#if defined(FFTW_SINGLE_ONLY)
  const bool align_ok = alignment_of(input) == alignment_of(m_input);
  if (align_ok && sz_ok)
  {
    run_plan_r2c(m_fw, input, m_output);
  }
  else if(!align_ok && sz_ok)
  {
    std::copy_n(input, m_size, m_input);
    run_plan_r2c(m_fw, m_input, m_output);
  }
  else
  {
    std::copy_n(input, sz, m_input);
    for(int i = sz; sz < m_size; i++)
    {
      m_input[i] = 0.f;
    }
    run_plan_r2c(m_fw, m_input, m_output);
  }
#else
  if(sz_ok)
  {
    std::copy_n(input, m_size, m_input);
    run_plan_r2c(m_fw, m_input, m_output);
  }
  else
  {
    std::copy_n(input, sz, m_input);
    for(int i = sz; sz < m_size; i++)
    {
      m_input[i] = 0.;
    }
    run_plan_r2c(m_fw, m_input, m_output);
  }
#endif

  return m_output;
}

fft_complex* fft::execute() noexcept
{
  run_plan_r2c(m_fw, m_input, m_output);

  return m_output;
}

rfft::rfft(std::size_t newSize) noexcept
{
  m_size = newSize;
  m_input = reinterpret_cast<fft_complex*>(alloc_complex(newSize / 2 + 1));
  m_output = alloc_real(newSize);
  m_fw = create_plan_c2r(newSize, m_input, m_output, FFTW_DESTROY_INPUT | FFTW_MEASURE);
}

rfft::~rfft()
{
  destroy_plan(m_fw);
  fft_free(m_input);
  fft_free(m_output);
  //  cleanup();
}

void rfft::reset(std::size_t newSize)
{
  this->~rfft();
  new(this) rfft(newSize);
}

fft_real* rfft::execute(fft_complex* input) noexcept
{
  run_plan_c2r(m_fw, input, m_output);

  return m_output;
}

fft_real* rfft::execute() noexcept
{
  run_plan_c2r(m_fw, m_input, m_output);

  return m_output;
}
}
#elif defined(OSSIA_FFT_KFR)
#include <kfr/kfr.h>
#include <kfr/dft.hpp>
#include <ossia/detail/pod_vector.hpp>

struct fftw_plan_s : kfr::dft_plan_real<double> {
  using dft_plan_real::dft_plan_real;
};

namespace ossia
{
static_assert(sizeof(kfr::c64) == sizeof(fft_complex));
static const constexpr auto fft_real_allocator = pod_allocator_avx2<kfr::f64>{};
static const constexpr auto fft_cplx_allocator = pod_allocator_avx2<kfr::c64>{};
static const constexpr auto alloc_real = [] (std::size_t sz) { return fft_real_allocator.allocate(sz); };
static const constexpr auto alloc_complex = [] (std::size_t sz) { return fft_cplx_allocator.allocate(sz); };
static const constexpr struct {
  void operator()(fft_real* p) const noexcept { fft_real_allocator.deallocate(p, 0); }
  void operator()(fft_complex* p) const noexcept { fft_cplx_allocator.deallocate(
          reinterpret_cast<kfr::c64*>(p), 0);
  }
} fft_free;
static const constexpr auto create_plan_r2c = [] (std::size_t sz, ...) { return new fftw_plan_s(sz); };
static const constexpr auto run_plan_r2c = [] (void* plan, fft_real* real, fft_complex* cplx, auto& temp) {
  return ((fftw_plan_s*)plan)->execute((kfr::c64*)cplx, (kfr::f64*)real, temp.data(), kfr::cdirect_t{});
};
static const constexpr auto create_plan_c2r = create_plan_r2c;
static const constexpr auto run_plan_c2r = [] (void* plan, fft_complex* cplx, fft_real* real, auto& temp) {
  return ((fftw_plan_s*)plan)->execute((kfr::f64*)real, (kfr::c64*)cplx, temp.data(), kfr::cinvert_t{});
};
static const constexpr auto destroy_plan = [] (auto&& p) { };
static const constexpr auto cleanup = [] { };
static const constexpr auto alignment_of = [] { };
static const constexpr auto FFTW_DESTROY_INPUT = 0;
static const constexpr auto FFTW_MEASURE = 0;
fft::fft(std::size_t newSize) noexcept
{
  m_size = newSize;
  m_input = alloc_real(newSize);
  m_output = reinterpret_cast<fft_complex*>(alloc_complex(newSize / 2 + 1));
  m_fw = create_plan_r2c(newSize, m_input, m_output, FFTW_DESTROY_INPUT | FFTW_MEASURE);
  m_storage.resize(((fftw_plan_s*)m_fw)->temp_size);
}

fft::~fft()
{
  destroy_plan(m_fw);
  fft_free(m_input);
  fft_free(m_output);
  //  cleanup();
}

void fft::reset(std::size_t newSize)
{
  this->~fft();
  new(this) fft(newSize);
}

fft_complex* fft::execute(float* input, std::size_t sz) noexcept
{
  const bool sz_ok = sz >= m_size;

  if(sz_ok)
  {
    std::copy_n(input, m_size, m_input);
    run_plan_r2c(m_fw, m_input, m_output, m_storage);
  }
  else
  {
    std::copy_n(input, sz, m_input);
    for(int i = sz; sz < m_size; i++)
    {
      m_input[i] = 0.;
    }
    run_plan_r2c(m_fw, m_input, m_output, m_storage);
  }

  return m_output;
}

fft_complex* fft::execute() noexcept
{
  run_plan_r2c(m_fw, m_input, m_output, m_storage);

  return m_output;
}

rfft::rfft(std::size_t newSize) noexcept
{
  m_size = newSize;
  m_input = reinterpret_cast<fft_complex*>(alloc_complex(newSize / 2 + 1));
  m_output = alloc_real(newSize);
  m_fw = create_plan_c2r(newSize, m_input, m_output, FFTW_DESTROY_INPUT | FFTW_MEASURE);
  m_storage.resize(((fftw_plan_s*)m_fw)->temp_size);
}

rfft::~rfft()
{
  destroy_plan(m_fw);
  fft_free(m_input);
  fft_free(m_output);
  //  cleanup();
}

void rfft::reset(std::size_t newSize)
{
  this->~rfft();
  new(this) rfft(newSize);
}

fft_real* rfft::execute(fft_complex* input) noexcept
{
  run_plan_c2r(m_fw, input, m_output, m_storage);

  return m_output;
}

fft_real* rfft::execute() noexcept
{
  run_plan_c2r(m_fw, m_input, m_output, m_storage);

  return m_output;
}
}

#endif
