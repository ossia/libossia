// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/ossia.hpp>
#include <ossia/detail/logger.hpp>

int main()
{
  using namespace ossia;

  // Create a device with a single address
  ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "app"};
  auto address = ossia::net::create_node(device, "/the_float").create_parameter(ossia::val_type::FLOAT);

  // Log a message when something is pushed on the address
  address->add_callback([] (const auto& value) {
    ossia::logger().info("Received value: {}", value);
  });

  // Create the temporal structure
  auto start_node = std::make_shared<time_node>();
  auto end_node = std::make_shared<time_node>();

  auto start_event = *(start_node->emplace(start_node->get_time_events().begin(), {}));
  auto end_event = *(end_node->emplace(end_node->get_time_events().begin(), {}));

  // Our process will play for five seconds
  auto constraint =
      std::make_shared<time_constraint>(
        [] (auto t, auto t2, const auto& state) { ossia::launch(state); },
        *start_event, *end_event, 5000._tv, 5000._tv, 5000._tv);

  ossia::clock clk{*constraint};
  using namespace std::literals;

  clk.set_granularity(50ms);
  clk.set_duration(5000._tv);

  // The curve that we want to play. Do one curve per address.
  auto behaviour = std::make_shared<curve<double, float>>();

  behaviour->set_x0(0.);
  behaviour->set_y0(0.);
  behaviour->add_point(curve_segment_linear<float>{}, 0.5, 1.);
  behaviour->add_point(curve_segment_ease<float, easing::circularOut>{}, 1., 0.);

  constraint->add_time_process(std::make_unique<automation>(*address, (curve_ptr)behaviour));

  constraint->set_speed(1._tv);

  // Start the execution. It runs in its own separate thread.
  std::cerr << "Starting\n";
  ossia::state st;
  clk.start(st);
  while (clk.running())
      ;
  clk.stop();

  std::cerr << "Starting again\n";
  // The execution resets to zero:
  clk.start(st);
  while (clk.running())
      ;
  clk.stop();

  std::cerr << "Starting manually\n";
  // We can have the execution perform manually, too,
  // for instance for use with an external clock source
  constraint->start(st);
  for(int i = 0; i < 500; i++)
  {
    // Tick every 100 units of time
    constraint->tick(10._tv);
  }
  constraint->stop();

  std::cerr << std::flush;
  return 0;
}
