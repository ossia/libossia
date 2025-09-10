#include <ossia/audio/fft.hpp>

#include <algorithm>
#include <new>

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
auto fft_free(void*) { }
auto destroy_plan(void*) { }
auto create_plan_r2c(std::size_t, void*, void*, int)
{
  return fft_plan{};
}
auto create_plan_c2r(std::size_t, void*, void*, int)
{
  return fft_plan{};
}
auto run_plan_r2c(fft_plan, void*, void*) { }
auto run_plan_c2r(fft_plan, void*, void*)
{
  return fft_plan{};
}
#endif
}

fft::fft(std::size_t newSize) noexcept
{
  m_size = newSize;
  if(std::popcount(newSize) == 1)
  {
    m_storage_size = m_size;
  }
  else
  {
    int power = 1;
    while(power < newSize)
      power *= 2;
    m_storage_size = power;
  }
  m_input = alloc_real(m_storage_size);
  m_output = reinterpret_cast<fft_complex*>(alloc_complex(m_storage_size / 2 + 1));
  m_fw = create_plan_r2c(
      m_storage_size, m_input, m_output, FFTW_DESTROY_INPUT | FFTW_MEASURE);
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
  if(align_ok && sz_ok)
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
    for(int i = sz; i < m_size; i++)
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
    for(int i = sz; i < m_size; i++)
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
  if(std::popcount(newSize) == 1)
  {
    m_storage_size = m_size;
  }
  else
  {
    int power = 1;
    while(power < newSize)
      power *= 2;
    m_storage_size = power;
  }
  m_input = reinterpret_cast<fft_complex*>(alloc_complex(m_storage_size / 2 + 1));
  m_output = alloc_real(m_storage_size);
  m_fw = create_plan_c2r(
      m_storage_size, m_input, m_output, FFTW_DESTROY_INPUT | FFTW_MEASURE);
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
#include <ossia/detail/pod_vector.hpp>

#include <kfr/dft.hpp>
#include <kfr/kfr.h>

struct fftw_plan_s : kfr::dft_plan_real<double>
{
  using dft_plan_real::dft_plan_real;
};

namespace ossia
{
static_assert(sizeof(kfr::c64) == sizeof(fft_complex));
static const constexpr auto fft_real_allocator = pod_allocator_avx2<kfr::f64>{};

static const constexpr auto alloc_real
    = [](std::size_t sz) { return fft_real_allocator.allocate(sz); };
// FIXME KFR 6: no more pod
static const constexpr auto alloc_complex
    = [](std::size_t sz) { return std::allocator<kfr::c64>{}.allocate(sz); };
static const constexpr struct
{
  void operator()(fft_real* p) const noexcept { fft_real_allocator.deallocate(p, 0); }
  void operator()(fft_complex* p) const noexcept
  {
    std::allocator<kfr::c64>{}.deallocate(reinterpret_cast<kfr::c64*>(p), 0);
  }
} fft_free;
static const constexpr auto create_plan_r2c
    = [](std::size_t sz, ...) { return new fftw_plan_s(sz); };
static const constexpr auto run_plan_r2c
    = [](void* plan, fft_real* real, fft_complex* cplx, auto& temp) {
  return ((fftw_plan_s*)plan)
      ->execute((kfr::c64*)cplx, (kfr::f64*)real, temp.data(), kfr::cdirect_t{});
};
static const constexpr auto create_plan_c2r = create_plan_r2c;
static const constexpr auto run_plan_c2r
    = [](void* plan, fft_complex* cplx, fft_real* real, auto& temp) {
  return ((fftw_plan_s*)plan)
      ->execute((kfr::f64*)real, (kfr::c64*)cplx, temp.data(), kfr::cinvert_t{});
};
static const constexpr auto destroy_plan = [](auto&& p) { delete(fftw_plan_s*)p; };
static const constexpr auto cleanup = [] {};
static const constexpr auto alignment_of = [] {};
static const constexpr auto FFTW_DESTROY_INPUT = 0;
static const constexpr auto FFTW_MEASURE = 0;
fft::fft(std::size_t newSize) noexcept
{
  m_size = newSize;
  if(std::popcount(newSize) == 1)
  {
    m_storage_size = m_size;
  }
  else
  {
    int power = 1;
    while(power < newSize)
      power *= 2;
    m_storage_size = power;
  }
  m_input = alloc_real(m_storage_size + 1);
  m_output = reinterpret_cast<fft_complex*>(alloc_complex(m_storage_size / 2 + 1));
  m_fw = create_plan_r2c(
      m_storage_size, m_input, m_output, FFTW_DESTROY_INPUT | FFTW_MEASURE);
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
    for(int i = sz; i < m_size; i++)
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
  if(std::popcount(newSize) == 1)
  {
    m_storage_size = m_size;
  }
  else
  {
    int power = 1;
    while(power < newSize)
      power *= 2;
    m_storage_size = power;
  }
  m_input = reinterpret_cast<fft_complex*>(alloc_complex(m_storage_size / 2 + 1));
  m_output = alloc_real(m_storage_size + 1);
  m_fw = create_plan_c2r(
      m_storage_size, m_input, m_output, FFTW_DESTROY_INPUT | FFTW_MEASURE);
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

#else
#include <ossia/detail/math.hpp>

#include <complex>

namespace ossia
{
namespace
{
// Minimal implementation taken from
// https://gist.github.com/lukicdarkoo/3f0d056e9244784f8b4a
template <typename FP>
static void fft_rec(std::complex<FP>* x, int N)
{
  using namespace std;
  using cplx = std::complex<FP>;

  // Check if it is splitted enough
  if(N <= 1)
  {
    return;
  }

  // Split even and odd
  auto* odd = (cplx*)alloca(sizeof(cplx) * N / 2);
  auto* even = (cplx*)alloca(sizeof(cplx) * N / 2);
  for(int i = 0; i < N / 2; i++)
  {
    even[i] = x[i * 2];
    odd[i] = x[i * 2 + 1];
  }

  // Split on tasks
  fft_rec(even, N / 2);
  fft_rec(odd, N / 2);

  // Calculate DFT
  for(int k = 0; k < N / 2; k++)
  {
    cplx t = exp(cplx(0, -2 * ossia::pi * k / N)) * odd[k];
    x[k] = even[k] + t;
    x[N / 2 + k] = even[k] - t;
  }
}

template <typename FP>
static void do_fft(FP* x_in, std::complex<FP>* x_out, int N)
{
  for(int i = 0; i < N; i++)
  {
    x_out[i] = {x_in[i], 0};
  }
  fft_rec(x_out, N);
}
}

fft::fft(std::size_t newSize) noexcept
{
  m_size = newSize;
  if(std::popcount(newSize) == 1)
  {
    m_storage_size = m_size;
  }
  else
  {
    int power = 1;
    while(power < newSize)
      power *= 2;
    m_storage_size = power;
  }
  m_input = (ossia::fft_real*)calloc(m_storage_size + 1, sizeof(ossia::fft_real));
  m_output = (ossia::fft_complex*)calloc(m_storage_size + 1, sizeof(ossia::fft_complex));
}

fft::~fft()
{
  free(m_input);
  free(m_output);
}

void fft::reset(std::size_t newSize)
{
  this->~fft();
  new(this) fft(newSize);
}

fft_complex* fft::execute(float* input, std::size_t sz) noexcept
{
  std::copy_n(input, m_size, m_input);
  do_fft(m_input, reinterpret_cast<std::complex<double>*>(m_output), sz);
  return m_output;
}

fft_complex* fft::execute() noexcept
{
  do_fft(m_input, reinterpret_cast<std::complex<double>*>(m_output), m_size);
  return m_output;
}

rfft::rfft(std::size_t newSize) noexcept
{
  m_size = newSize;
  if(std::popcount(newSize) == 1)
  {
    m_storage_size = m_size;
  }
  else
  {
    int power = 1;
    while(power < newSize)
      power *= 2;
    m_storage_size = power;
  }
  m_input = (ossia::fft_complex*)calloc(newSize + 1, sizeof(ossia::fft_complex));
  m_output = (ossia::fft_real*)calloc(newSize + 1, sizeof(ossia::fft_real));
}

rfft::~rfft()
{
  free(m_input);
  free(m_output);
}

void rfft::reset(std::size_t newSize)
{
  this->~rfft();
  new(this) rfft(newSize);
}

fft_real* rfft::execute(fft_complex* input) noexcept
{
  // TODO
  return m_output;
}

fft_real* rfft::execute() noexcept
{
  // TODO
  return m_output;
}
}
#endif
