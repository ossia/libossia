// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/ossia.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <benchmark/benchmark.h>


static void BM_SomeFunction(benchmark::State& state) {
  // Perform setup here
  for (auto _ : state) {

      state.PauseTiming();
      int k = state.range(0);
      ossia::net::generic_device src{std::make_unique<ossia::oscquery::oscquery_server_protocol>(1122, 5566), "dev"};
      for(int i = 0; i < k; i++)
      {
        auto n = src.create_child(std::to_string(i));
        n->create_parameter(ossia::val_type::FLOAT);
      }

      auto proto = new ossia::oscquery::oscquery_mirror_protocol("ws://127.0.0.1:5566");
      ossia::net::generic_device dest{std::unique_ptr<ossia::net::protocol_base>(proto), "dev"};
      state.ResumeTiming();

      auto fut = proto->update_async(dest);
      fut.wait();
  }
}
// Register the function as a benchmark
BENCHMARK(BM_SomeFunction)->DenseRange(0, 500, 50);
// Run the benchmark
BENCHMARK_MAIN();
