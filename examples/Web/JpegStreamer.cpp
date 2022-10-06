#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <websocketpp/base64/base64.hpp>

int main(int, char**)
{
  using namespace ossia::net;
  // Create a device which will listen on the websocket port 5678 and osc port 1234
  generic_device device{
      std::make_unique<ossia::oscquery::oscquery_server_protocol>(1234, 5678),
      "my_device"};

  auto& node = find_or_create_node(device, "/jpeg");
  auto param = node.create_parameter(ossia::val_type::STRING);

  std::vector<int> compression_params;
  compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
  compression_params.push_back(20);

  cv::Mat small, mat;
  cv::VideoCapture cap;
  std::vector<uchar> jpeg_buf{};

  while(true)
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
        param->push_value(encoded);
      }
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(30ms);
    }
  }

  return 0;
}
