#include <ossia/audio/fft.hpp>
#include <fftw3.h>

namespace ossia
{
namespace
{
#if defined(FFTW_SINGLE_ONLY)
  static const constexpr auto alloc_real = ::fftwf_alloc_real;
  static const constexpr auto alloc_complex = ::fftwf_alloc_complex;
  static const constexpr auto fft_free = ::fftwf_free;
  static const constexpr auto create_plan = ::fftwf_plan_dft_r2c_1d;
  static const constexpr auto run_plan = ::fftwf_execute_dft_r2c;
  static const constexpr auto destroy_plan = ::fftwf_destroy_plan;
  static const constexpr auto cleanup = ::fftwf_cleanup;
  static const constexpr auto alignment_of = ::fftwf_alignment_of;

#elif defined(FFTW_DOUBLE_ONLY)
  static const constexpr auto alloc_real = ::fftw_alloc_real;
  static const constexpr auto alloc_complex = ::fftw_alloc_complex;
  static const constexpr auto fft_free = ::fftw_free;
  static const constexpr auto create_plan = ::fftw_plan_dft_r2c_1d;
  static const constexpr auto run_plan = ::fftw_execute_dft_r2c;
  static const constexpr auto destroy_plan = ::fftw_destroy_plan;
  static const constexpr auto cleanup = ::fftw_cleanup;
  static const constexpr auto alignment_of = ::fftw_alignment_of;
#endif
}

fft::fft(std::size_t newSize) noexcept
{
  m_size = newSize;
  m_input = alloc_real(newSize);
  m_output = alloc_complex(newSize / 2 + 1);
  m_fw = create_plan(newSize, m_input, m_output, FFTW_DESTROY_INPUT | FFTW_MEASURE);
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

fft::fft_complex* fft::execute(float* input, std::size_t sz) noexcept
{
  const bool sz_ok = sz >= m_size;

#if defined(FFTW_SINGLE_ONLY)
  const bool align_ok = alignment_of(input) == alignment_of(m_input);
  if (align_ok && sz_ok)
  {
    run_plan(m_fw, input, m_output);
  }
  else if(!align_ok && sz_ok)
  {
    std::copy_n(input, m_size, m_input);
    run_plan(m_fw, m_input, m_output);
  }
  else
  {
    std::copy_n(input, sz, m_input);
    for(int i = sz; sz < m_size; i++)
    {
      m_input[i] = 0.f;
    }
    run_plan(m_fw, m_input, m_output);
  }
#else
  if(sz_ok)
  {
    std::copy_n(input, m_size, m_input);
    run_plan(m_fw, m_input, m_output);
  }
  else
  {
    std::copy_n(input, sz, m_input);
    for(int i = sz; sz < m_size; i++)
    {
      m_input[i] = 0.;
    }
    run_plan(m_fw, m_input, m_output);
  }
#endif

  return m_output;
}

}
