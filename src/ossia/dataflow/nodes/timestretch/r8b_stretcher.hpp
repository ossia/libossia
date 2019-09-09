#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/token_request.hpp>
#include <ossia/dataflow/nodes/media.hpp>

#if __has_include(<CDSPResampler.h>)
#include <CDSPResampler.h>
#include <r8bbase.cpp>
#else
#include <r8brain-free-src/CDSPResampler.h>
#endif
namespace ossia
{

struct r8b_stretcher
{
//  void run_repitch_r8b(
//      const ossia::token_request& t,
//      ossia::exec_state_facade e,
//      const std::size_t chan,
//      const std::size_t len,
//      const int64_t samples_to_read,
//      const int64_t samples_to_write,
//      ossia::audio_port& ap) noexcept
//  {
//    // speed == 0.5 -> 2 times more samples
//
//    double target_rate = this->m_fileSampleRate / t.speed;
//
//    std::vector<std::unique_ptr<r8b::CDSPResampler24>> resamplers;
//    std::vector<std::pair<double*, int>> lastResampled;
//    for (std::size_t i = 0; i < channels(); ++i)
//    {
//      /* Create the resampler objects. */
//      ap.samples[i].resize(t.offset.impl + samples_to_write);
//
//      if(resamplers.size() < channels())
//      {
//        lastResampled.push_back({});
//        resamplers.emplace_back(std::make_unique<r8b::CDSPResampler24>(this->m_fileSampleRate, target_rate, std::max(int64_t(16384), samples_to_read)));
//
//      }
//      double* input = const_cast<double*>(&m_data[i][t.prev_date]);
//      double* output = nullptr;
//      int res = resamplers[i]->process(input, samples_to_read, output);
//
//      std::cerr << "read: " << samples_to_read
//                << " ; write: " << samples_to_write
//                << " ; rate: " << target_rate
//                << " ; res: " << res
//                << std::endl;
//      for(int j = 0; j < samples_to_write; j++)
//      {
//        ap.samples[i][j + t.offset] = output[j];
//      }
//      do_fade(
//          t.start_discontinuous, t.end_discontinuous, ap.samples[i],
//          t.offset.impl, samples_to_write);
//    }
//  }
};
}
