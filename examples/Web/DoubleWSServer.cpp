#include <ossia/detail/config.hpp>

#include <ossia/detail/base64.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/sockets/websocket_server_beast.hpp>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <spdlog/sinks/stdout_sinks.h>

#include <boost/asio/post.hpp>

#include <atomic>
#include <iostream>
#include <set>

// double websocket server test
// this example create a simple oscquery server that exposes its parameter through port 5678
// you can get the namespace in a browser with : ws://127.0.0.1:5678
// and it also creates a second web socket server to stream a webcam in JPEG on port 9003
// to see it, open the file double_ws_server-test.html (next to this one) in a browser

using namespace std;
using namespace ossia;

class broadcast_server
{
public:
  explicit broadcast_server(ossia::net::network_context_ptr ctx)
      : m_ctx{std::move(ctx)}
      , m_server{m_ctx}
  {
    using namespace ossia::net;
    m_server.set_open_handler(
        [this](ws_connection_handle hdl) { m_connections.insert(hdl); });
    m_server.set_close_handler(
        [this](ws_connection_handle hdl) { m_connections.erase(hdl); });
    m_server.set_message_handler(
        [](const ws_connection_handle&, ws_opcode, const std::string&) {
      return server_reply{};
    });

    m_cap_thread = std::thread([this]() {
      std::vector<int> compression_params;
      compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
      compression_params.push_back(20);

      cv::Mat small, mat;
      cv::VideoCapture cap;
      std::vector<uchar> jpeg_buf{};

      while(!m_quit)
      {
        if(!cap.isOpened())
          cap.open(0);
        else
        {
          cap >> mat;

          cv::resize(mat, small, cv::Size(), 0.5, 0.5);
          if(cv::imencode(".jpg", small, jpeg_buf, compression_params))
          {
            std::string encoded
                = ossia::base64_encode(jpeg_buf.data(), jpeg_buf.size());

            // The connection list and the actual writes are handled on the
            // server's io_context thread so we don't race with the strand
            // that runs accept / read / close handlers.
            boost::asio::post(m_ctx->context, [this, msg = std::move(encoded)] {
              for(auto& hdl : m_connections)
                m_server.send_message(hdl, msg);
            });
          }
          usleep(30000);
        }
      }
    });
  }

  ~broadcast_server()
  {
    m_quit = true;
    if(m_cap_thread.joinable())
      m_cap_thread.join();
  }

  void run(uint16_t port)
  {
    m_server.listen(port);
    m_server.run();
  }

private:
  using con_list = std::set<
      ossia::net::ws_connection_handle,
      std::owner_less<ossia::net::ws_connection_handle>>;

  ossia::net::network_context_ptr m_ctx;
  ossia::net::websocket_server_beast m_server;
  con_list m_connections;

  std::atomic_bool m_quit{false};
  std::thread m_cap_thread;
};

void printValueCallback(const value& v)
{
  cerr << "Callback: " << value_to_pretty_string(v) << "\n";
}

int main()
{
  using namespace ossia::net;
  // Create a device which will listen on the websocket port 5678 and osc port 1234
  generic_device device{
      std::make_unique<ossia::oscquery::oscquery_server_protocol>(1234, 5678),
      "my_device"};

  // Create a few float parameters
  std::vector<parameter_base*> my_params;
  for(int i = 0; i < 10; i++)
  {
    auto& node = find_or_create_node(device, "/test/foo." + std::to_string(i));
    auto param = node.create_parameter(ossia::val_type::FLOAT);
    param->push_value(0.1 + 0.01 * i);
    param->add_callback(printValueCallback);

    my_params.push_back(param);
  }

  std::thread push_thread([&]() {
    while(true)
    {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(1000ms);

      // Update the values of the parameters with a chaotic function regularly
      for(auto param : my_params)
      {
        const auto v = param->value().get<float>();
        param->push_value(3.7f * v * (1.f - v));
      }
    }
  });
  push_thread.detach();

  auto ctx = std::make_shared<ossia::net::network_context>();
  broadcast_server server{ctx};
  server.run(9003);
}
