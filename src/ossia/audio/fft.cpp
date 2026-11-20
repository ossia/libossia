#include <ossia/audio/fft.hpp>

#include <algorithm>
#include <bit>
#include <cassert>
#include <new>
#include <stdexcept>

#if defined(OSSIA_FFT_FFTW)
#include <fftw3.h>
#include <limits>
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
static const constexpr auto create_plan_complex = ::fftwf_plan_dft_1d;
static const constexpr auto run_plan = ::fftwf_execute;
static const constexpr auto destroy_plan = ::fftwf_destroy_plan;
static const constexpr auto cleanup = ::fftwf_cleanup;

#elif defined(OSSIA_FFTW_DOUBLE_ONLY)
static const constexpr auto alloc_real = ::fftw_alloc_real;
static const constexpr auto alloc_complex = ::fftw_alloc_complex;
static const constexpr auto fft_free = ::fftw_free;
static const constexpr auto create_plan_r2c = ::fftw_plan_dft_r2c_1d;
static const constexpr auto run_plan_r2c = ::fftw_execute_dft_r2c;
static const constexpr auto create_plan_c2r = ::fftw_plan_dft_c2r_1d;
static const constexpr auto run_plan_c2r = ::fftw_execute_dft_c2r;
static const constexpr auto create_plan_complex = ::fftw_plan_dft_1d;
static const constexpr auto run_plan = ::fftw_execute;
static const constexpr auto destroy_plan = ::fftw_destroy_plan;
// static const constexpr auto cleanup = ::fftw_cleanup;
#else
#define FFTW_DESTROY_INPUT 0
#define FFTW_MEASURE 0
#define FFTW_ESTIMATE 0

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

void fft::build(std::size_t newSize, bool measure) noexcept
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
  // Measuring runs transforms to choose the plan: not on the audio thread,
  // where a size nothing was prepared for has to be planned
  m_fw = create_plan_r2c(
      m_storage_size, m_input, m_output,
      FFTW_DESTROY_INPUT | (measure ? FFTW_MEASURE : FFTW_ESTIMATE));
}

void fft::release() noexcept
{
  destroy_plan(m_fw);
  fft_free(m_input);
  fft_free(m_output);
  //  cleanup();
}

fft_complex* fft::execute(float* input, std::size_t sz) noexcept
{
  // The plan destroys its input and covers the power-of-two storage: always
  // transform a zero-padded copy.
  const std::size_t n = std::min(sz, m_size);
  std::copy_n(input, n, m_input);
  std::fill(m_input + n, m_input + m_storage_size, 0);
  run_plan_r2c(m_fw, m_input, m_output);

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
#include <variant>

struct fftw_plan_s : kfr::dft_plan_real<double>
{
  using dft_plan_real::dft_plan_real;
};

namespace ossia
{
static_assert(sizeof(kfr::c64) == sizeof(fft_complex));
static const constexpr auto fft_real_allocator = pod_allocator_avx2<kfr::f64>{};
static const constexpr auto fft_complex_allocator = pod_allocator_avx2<_Complex double>{};

static const constexpr auto alloc_real
    = [](std::size_t sz) { return fft_real_allocator.allocate(sz); };
// FIXME KFR 6: no more pod
static const constexpr auto alloc_complex
    = [](std::size_t sz) {
  return fft_complex_allocator.allocate(sz);
};
static const constexpr struct
{
  // FIXME we should pass the actual size allocated, not zero.
  // It only works because pod_allocator_avx2 does not care, but modern std::allocator
  // cares
  void operator()(fft_real* p) const noexcept { fft_real_allocator.deallocate(p, 0); }
  void operator()(fft_complex* p) const noexcept
  {
    fft_complex_allocator.deallocate(reinterpret_cast<_Complex double*>(p), 0);
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
static const constexpr auto FFTW_DESTROY_INPUT = 0;
static const constexpr auto FFTW_MEASURE = 0;
void fft::build(std::size_t newSize, bool /*measure*/) noexcept
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

void fft::release() noexcept
{
  destroy_plan(m_fw);
  fft_free(m_input);
  fft_free(m_output);
  //  cleanup();
}

fft_complex* fft::execute(float* input, std::size_t sz) noexcept
{
  const std::size_t n = std::min(sz, m_size);
  std::copy_n(input, n, m_input);
  std::fill(m_input + n, m_input + m_storage_size, 0);
  run_plan_r2c(m_fw, m_input, m_output, m_storage);

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
#include <ossia/audio/fft_exact_portable.hpp>

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

void fft::build(std::size_t newSize, bool /*measure*/) noexcept
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

void fft::release() noexcept
{
  free(m_input);
  free(m_output);
}

fft_complex* fft::execute(float* input, std::size_t sz) noexcept
{
  const std::size_t n = std::min(sz, m_size);
  std::copy_n(input, n, m_input);
  std::fill(m_input + n, m_input + m_storage_size, 0);
  do_fft(m_input, reinterpret_cast<std::complex<double>*>(m_output), m_storage_size);
  return m_output;
}

fft_complex* fft::execute() noexcept
{
  do_fft(m_input, reinterpret_cast<std::complex<double>*>(m_output), m_storage_size);
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

namespace ossia
{
namespace
{
std::size_t storage_for(std::size_t size) noexcept
{
  return std::bit_ceil(std::max<std::size_t>(size, 1));
}
}

fft::fft(std::size_t newSize) noexcept
{
  build(newSize, true);
}

fft::~fft()
{
  release();
  for(auto& s : m_spares)
  {
    if(!s.input)
      continue;
    swap(s);
    release();
  }
}

void fft::swap(spare& s) noexcept
{
  std::swap(m_fw, s.fw);
  std::swap(m_size, s.size);
  std::swap(m_storage_size, s.storage_size);
  std::swap(m_input, s.input);
  std::swap(m_output, s.output);
  std::swap(m_storage, s.storage);
}

void fft::reset(std::size_t newSize)
{
  use(newSize, false);
}

void fft::prepare(std::size_t newSize)
{
  // The sizes an audio server most often switches to, around its own
  const std::size_t storage = storage_for(newSize);
  for(std::size_t other : {storage / 4, storage / 2, storage * 2})
    if(other >= 16)
      use(other, true);
  use(newSize, true);
}

void fft::use(std::size_t newSize, bool measure)
{
  // The plan covers the power-of-two storage, whatever the size within it
  const std::size_t storage = storage_for(newSize);
  if(storage == m_storage_size)
  {
    m_size = newSize;
    return;
  }

  for(auto& s : m_spares)
  {
    if(s.input && s.storage_size == storage)
    {
      swap(s);
      s.used = ++m_uses;
      m_size = newSize;
      return;
    }
  }

  // Keep the current plan for when the size comes back, in a free slot or
  // in place of the one unused for the longest
  auto slot = std::find_if(
      m_spares.begin(), m_spares.end(), [](const spare& s) { return !s.input; });
  if(slot == m_spares.end())
  {
    slot = std::min_element(
        m_spares.begin(), m_spares.end(),
        [](const spare& a, const spare& b) { return a.used < b.used; });
    // That one is freed, the current one takes its place
    swap(*slot);
    release();
  }
  else
  {
    swap(*slot);
  }
  slot->used = ++m_uses;
  m_fw = {};
  m_input = nullptr;
  m_output = nullptr;
  build(newSize, measure);
}
}

namespace ossia
{
namespace
{
using complex_sample = std::complex<float>;
using fft_kind = exact_fft::kind;

bool inverse(fft_kind mode) noexcept
{
  return mode == fft_kind::real_inverse || mode == fft_kind::complex_inverse;
}

#if defined(OSSIA_FFT_FFTW) || defined(OSSIA_FFT_KFR)
bool real_transform(fft_kind mode) noexcept
{
  return mode == fft_kind::real_inverse || mode == fft_kind::real_forward;
}
#endif

#if !defined(OSSIA_FFT_FFTW)
void expand_hermitian(
    std::span<const complex_sample> input, std::span<complex_sample> output) noexcept
{
  const auto n = output.size();
  output[0] = input[0].real();
  for(std::size_t i = 1; i <= (n - 1) / 2; ++i)
  {
    output[i] = input[i];
    output[n - i] = std::conj(input[i]);
  }
  if(n % 2 == 0)
    output[n / 2] = input[n / 2].real();
}
#endif
}

struct exact_fft::plan
{
  const std::size_t size;
  const kind mode;

#if defined(OSSIA_FFT_FFTW)
  std::unique_ptr<fft_real, decltype(fft_free)> time{nullptr, fft_free};
  std::unique_ptr<fft_complex, decltype(fft_free)> frequency{nullptr, fft_free};
  std::unique_ptr<fft_complex, decltype(fft_free)> signal{nullptr, fft_free};
  fft_plan native{};

  plan(std::size_t n, kind k) : size{n}, mode{k}
  {
    if(n > std::size_t(std::numeric_limits<int>::max()))
      throw std::length_error("FFTW length exceeds INT_MAX");
    const bool real = real_transform(k);
    frequency.reset(alloc_complex(real ? n / 2 + 1 : n));
    if(real)
      time.reset(alloc_real(n));
    else
      signal.reset(alloc_complex(n));
    if(!frequency || (real ? !time : !signal))
      throw std::bad_alloc{};

    constexpr unsigned flags = FFTW_ESTIMATE | FFTW_DESTROY_INPUT;
    if(k == kind::real_forward)
      native = create_plan_r2c(int(n), time.get(), frequency.get(), flags);
    else if(k == kind::real_inverse)
      native = create_plan_c2r(int(n), frequency.get(), time.get(), flags);
    else
      native = create_plan_complex(
          int(n), signal.get(), frequency.get(),
          inverse(k) ? FFTW_BACKWARD : FFTW_FORWARD, flags);
    if(!native)
      throw std::runtime_error("Could not create an FFTW plan");
  }

  ~plan() { destroy_plan(native); }

  static void copy_in(std::span<const complex_sample> in, fft_complex* out) noexcept
  {
    for(std::size_t i = 0; i < in.size(); ++i)
    {
      out[i][0] = in[i].real();
      out[i][1] = in[i].imag();
    }
  }

  void copy_bins(std::span<complex_sample> out, float scale) const noexcept
  {
    const auto* bins = frequency.get();
    for(std::size_t i = 0; i < out.size(); ++i)
      out[i] = {float(bins[i][0] * scale), float(bins[i][1] * scale)};
  }

  void forward(std::span<const float> in, std::span<complex_sample> out) noexcept
  {
    std::copy(in.begin(), in.end(), time.get());
    run_plan(native);
    copy_bins(out, 1.f);
  }

  void backward(
      std::span<const complex_sample> in, std::span<float> out, float scale) noexcept
  {
    copy_in(in, frequency.get());
    run_plan(native);
    const auto* result = time.get();
    for(std::size_t i = 0; i < out.size(); ++i)
      out[i] = float(result[i] * scale);
  }

  void complex(
      std::span<const complex_sample> in, std::span<complex_sample> out,
      float scale) noexcept
  {
    copy_in(in, signal.get());
    run_plan(native);
    copy_bins(out, scale);
  }
#elif defined(OSSIA_FFT_KFR)
  using real_plan = kfr::dft_plan_real<float>;
  using complex_plan = kfr::dft_plan<float>;
  std::variant<real_plan, complex_plan> native;
  kfr::univector<kfr::u8> scratch;
  kfr::univector<complex_sample> input, output;

  plan(std::size_t n, kind k) : size{n}, mode{k}
  {
    if(real_transform(k) && n % 2 == 0)
    {
      scratch.resize(native.emplace<real_plan>(n).temp_size);
    }
    else
    {
      scratch.resize(native.emplace<complex_plan>(n).temp_size);
      if(real_transform(k))
        input.resize(n);
      if(k != kind::complex_inverse)
        output.resize(n);
    }
  }

  void forward(std::span<const float> in, std::span<complex_sample> out) noexcept
  {
    if(auto* p = std::get_if<real_plan>(&native))
    {
      p->execute(out.data(), in.data(), scratch.data(), kfr::cdirect_t{});
    }
    else
    {
      std::copy(in.begin(), in.end(), input.begin());
      std::get<complex_plan>(native).execute(
          output.data(), input.data(), scratch.data(), kfr::cfalse);
      std::copy_n(output.begin(), out.size(), out.begin());
    }
  }

  void backward(
      std::span<const complex_sample> in, std::span<float> out, float scale) noexcept
  {
    if(auto* p = std::get_if<real_plan>(&native))
    {
      p->execute(out.data(), in.data(), scratch.data(), kfr::cinvert_t{});
      if(scale != 1.f)
        for(auto& value : out)
          value *= scale;
    }
    else
    {
      expand_hermitian(in, {input.data(), size});
      std::get<complex_plan>(native).execute(
          output.data(), input.data(), scratch.data(), kfr::ctrue);
      for(std::size_t i = 0; i < out.size(); ++i)
        out[i] = output[i].real() * scale;
    }
  }

  void complex(
      std::span<const complex_sample> in, std::span<complex_sample> out,
      float scale) noexcept
  {
    auto& p = std::get<complex_plan>(native);
    auto* result = out.size() == size ? out.data() : output.data();
    if(mode == kind::complex_inverse)
      p.execute(result, in.data(), scratch.data(), kfr::ctrue);
    else
      p.execute(result, in.data(), scratch.data(), kfr::cfalse);
    if(result != out.data() || scale != 1.f)
      for(std::size_t i = 0; i < out.size(); ++i)
        out[i] = result[i] * scale;
  }
#else
  detail::exact_fft_portable native;

  plan(std::size_t n, kind k) : size{n}, mode{k}, native{n, inverse(k)} { }

  void forward(std::span<const float> in, std::span<complex_sample> out) noexcept
  {
    std::copy(in.begin(), in.end(), native.input().begin());
    native.execute();
    std::copy_n(native.output().begin(), out.size(), out.begin());
  }

  void backward(
      std::span<const complex_sample> in, std::span<float> out, float scale) noexcept
  {
    expand_hermitian(in, native.input());
    native.execute();
    const auto result = native.output();
    for(std::size_t i = 0; i < out.size(); ++i)
      out[i] = result[i].real() * scale;
  }

  void complex(
      std::span<const complex_sample> in, std::span<complex_sample> out,
      float scale) noexcept
  {
    std::copy(in.begin(), in.end(), native.input().begin());
    native.execute();
    const auto result = native.output();
    for(std::size_t i = 0; i < out.size(); ++i)
      out[i] = result[i] * scale;
  }
#endif
};

exact_fft::exact_fft(std::size_t size, kind mode)
{
  if(size == 0)
    throw std::invalid_argument("A transform must have a nonzero length");
  m_plan = std::make_unique<plan>(size, mode);
}

exact_fft::~exact_fft() = default;

void exact_fft::execute(
    std::span<const float> input, std::span<complex_sample> output) noexcept
{
  assert(m_plan->mode == kind::real_forward);
  assert(input.size() == m_plan->size && output.size() == input.size() / 2 + 1);
  m_plan->forward(input, output);
}

void exact_fft::execute(
    std::span<const complex_sample> input, std::span<float> output, float scale) noexcept
{
  assert(m_plan->mode == kind::real_inverse);
  assert(output.size() == m_plan->size && input.size() == output.size() / 2 + 1);
  m_plan->backward(input, output, scale);
}

void exact_fft::execute(
    std::span<const complex_sample> input, std::span<complex_sample> output,
    float scale) noexcept
{
  assert(m_plan->mode == kind::complex_forward || m_plan->mode == kind::complex_inverse);
  assert(input.size() == m_plan->size && output.size() <= input.size());
  assert(m_plan->mode == kind::complex_forward || output.size() == input.size());
  m_plan->complex(input, output, scale);
}
}
