#include <ossia/detail/config.hpp>

#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <spdlog/sinks/stdout_sinks.h>
#include <websocketpp/base64/base64.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <set>

// double websocket server test
// this example create a simple oscquery server that exposes its parameter through port 5678
// you can get the namespace in a browser with : ws://127.0.0.1:5678
// and it also creates a second web socket server to stream a webcam in JPEG on port 9003
// to see it, open the file double_ws_server-test.html (next to this one) in a browser
// KNOWN issue : as soon as you start the second ws server, the first one doesn't respond anymore

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

using namespace std;
using namespace ossia;

class broadcast_server
{
public:
  broadcast_server()
  {
    m_server.init_asio();

    m_server.set_open_handler(bind(&broadcast_server::on_open, this, ::_1));
    m_server.set_close_handler(bind(&broadcast_server::on_close, this, ::_1));
    m_server.set_message_handler(bind(&broadcast_server::on_message, this, ::_1, ::_2));

    m_server.clear_access_channels(
        websocketpp::log::alevel::frame_header
        | websocketpp::log::alevel::frame_payload);

    m_cap_thread = std::thread([&]() {
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
                = websocketpp::base64_encode(jpeg_buf.data(), jpeg_buf.size());

            con_list::iterator it;
            for(it = m_connections.begin(); it != m_connections.end(); ++it)
            {
              m_server.send(*it, encoded, websocketpp::frame::opcode::text);
            }
          }
          usleep(30000);
        }
      }
    });
  }

  void on_open(connection_hdl hdl) { m_connections.insert(hdl); }

  void on_close(connection_hdl hdl) { m_connections.erase(hdl); }

  void on_message(connection_hdl hdl, server::message_ptr msg)
  {
    //for (auto it : m_connections) {
    //    m_server.send(it,msg);
    // }
  }

  void run(uint16_t port)
  {
    m_server.listen(port);
    m_server.start_accept();
    m_server.run();
  }

private:
  typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;

  server m_server;
  con_list m_connections;

  bool m_quit = false;
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

  broadcast_server server;
  // comment the following to make ossia ws server work again
  server.run(9003);

  while(true)
    ;
}
