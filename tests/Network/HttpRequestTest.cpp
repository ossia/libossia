// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <catch.hpp>
#include <ossia/detail/config.hpp>
#include <ossia/network/oscquery/detail/http_client.hpp>
using namespace ossia;
std::atomic_bool running{};
struct http_answer
{

  template <typename T>
  void operator()(T& req, const std::string& str)
  {
      std::cerr << str << std::endl;
      req.close();
      running = false;
  }
};

struct http_error
{
  template <typename T>
  void operator()(T& req)
  {
    req.close();
    std::exit(1);
  }
};

static const std::string rep = "HTTP/1.1 200 OK\r\n"
                              "Content-Length: 2\r\n"
                              "Content-Type: text/html\r\n"
                              "Last-Modified: Sun, 12 Jan 2020 22:07:10 GMT\r\n"
                              "Accept-Ranges: bytes\r\n"
                              "Date: Sun, 12 Jan 2020 22:07:10 GMT\r\n"
                              "\r\n"
                              "hi"
;


void http_server()
{
    using asio::ip::tcp;
  try
  {
    asio::io_service io_service;

    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8089));

    for (;;)
    {
      running = true;
      tcp::socket socket(io_service);
      acceptor.accept(socket);

      asio::write(socket, asio::buffer(rep), asio::transfer_all());
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    std::exit(1);
  }
}

TEST_CASE ("test_http_request", "test_http_request")
{
    running = false;
    std::thread server{http_server};
    while(! running);

    asio::io_context context;
    auto worker = std::make_shared<asio::io_service::work>(context);
    using http_request = ossia::oscquery::http_get_request<http_answer, http_error>;
    auto req = std::make_shared<http_request>(http_answer{}, http_error{}, context, "http://127.0.0.1", "/");
    req->resolve("127.0.0.1", "8089");

    context.run();

//REQUIRE(res == expected);
}
