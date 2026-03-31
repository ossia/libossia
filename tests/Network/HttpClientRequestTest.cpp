
#include <ossia/detail/config.hpp>
#include <ossia/network/http/http_client_request.hpp>

#include "include_catch.hpp"

#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace asio = boost::asio;
using asio::ip::tcp;

// ---- Tiny embedded HTTP server for testing ----

struct http_test_server
{
  // What the server will send as response (set before accepting)
  std::string response;

  // What the server received from the client (readable after request completes)
  std::string received_request;

  std::atomic_bool ready{false};
  std::atomic_bool done{false};
  uint16_t port{0};

  asio::io_context ctx;
  tcp::acceptor acceptor{ctx};

  http_test_server() = default;

  void start()
  {
    // Bind to ephemeral port
    tcp::endpoint ep(tcp::v4(), 0);
    acceptor.open(ep.protocol());
    acceptor.set_option(tcp::acceptor::reuse_address(true));
    acceptor.bind(ep);
    acceptor.listen(1);
    port = acceptor.local_endpoint().port();
    ready = true;
  }

  // Accept one connection, read the request, send the response, close.
  void serve_one()
  {
    tcp::socket socket(ctx);
    acceptor.accept(socket);

    // Read request until we see the blank line ending headers
    asio::streambuf buf;
    asio::read_until(socket, buf, "\r\n\r\n");

    // Check if there's a Content-Length and read body
    std::string req_headers(
        asio::buffers_begin(buf.data()), asio::buffers_end(buf.data()));
    received_request = req_headers;

    auto cl_pos = req_headers.find("Content-Length: ");
    if(cl_pos != std::string::npos)
    {
      auto cl_end = req_headers.find("\r\n", cl_pos);
      int content_length
          = std::stoi(req_headers.substr(cl_pos + 16, cl_end - cl_pos - 16));

      // How much body is already in the buffer after headers
      auto header_end = req_headers.find("\r\n\r\n");
      int already_read = (int)req_headers.size() - (int)header_end - 4;
      int remaining = content_length - already_read;

      if(remaining > 0)
      {
        asio::streambuf body_buf;
        asio::read(socket, body_buf, asio::transfer_exactly(remaining));
        std::string body(
            asio::buffers_begin(body_buf.data()),
            asio::buffers_end(body_buf.data()));
        received_request += body;
      }
    }

    // Send response
    boost::system::error_code ec;
    asio::write(socket, asio::buffer(response), asio::transfer_all(), ec);
    socket.shutdown(tcp::socket::shutdown_both, ec);
    socket.close(ec);
    done = true;
  }

  void stop() { acceptor.close(); }
};

// Run the client request and block until it completes (or times out)
struct client_result
{
  int status{0};
  std::string body;
  std::string error;
  bool got_response{false};
  bool got_error{false};
};

struct test_answer
{
  static constexpr int reserve_expect = 65536;
  client_result* result;
  std::atomic_bool* finished;
  void operator()(auto& req, int status, std::string_view str)
  {
    result->status = status;
    result->body = std::string(str);
    result->got_response = true;
    *finished = true;
  }
};

struct test_error
{
  client_result* result;
  std::atomic_bool* finished;
  void operator()(auto& req, std::string_view msg)
  {
    result->error = std::string(msg);
    result->got_error = true;
    *finished = true;
  }
};

using test_request = ossia::net::http_client_request<test_answer, test_error>;

static client_result do_request(
    http_test_server& srv, std::string_view verb, std::string_view path,
    const std::vector<std::pair<std::string, std::string>>& headers = {},
    std::string_view body = {})
{
  client_result res;
  std::atomic_bool finished{false};

  asio::io_context client_ctx;
  auto host = "127.0.0.1";

  auto req = std::make_shared<test_request>(
      test_answer{&res, &finished}, test_error{&res, &finished}, client_ctx, verb,
      host, path, headers, body);

  req->resolve(host, std::to_string(srv.port));

  // Run server in a thread
  std::thread server_thread([&srv] { srv.serve_one(); });

  // Run client io_context (blocks until all async ops complete)
  client_ctx.run();

  server_thread.join();

  return res;
}

// ---- Helper to build standard HTTP responses ----

static std::string make_response(int status, const std::string& status_text,
    const std::string& body = {},
    const std::vector<std::pair<std::string, std::string>>& headers = {})
{
  std::string resp = "HTTP/1.1 " + std::to_string(status) + " " + status_text + "\r\n";
  bool has_content_length = false;
  for(auto& [k, v] : headers)
  {
    resp += k + ": " + v + "\r\n";
    if(k == "Content-Length")
      has_content_length = true;
  }
  if(!has_content_length)
    resp += "Content-Length: " + std::to_string(body.size()) + "\r\n";
  resp += "Connection: close\r\n";
  resp += "\r\n";
  resp += body;
  return resp;
}

// ===========================================================================
// Tests
// ===========================================================================

// ---- Basic GET ----

TEST_CASE("http_client_get_200", "http_client_get_200")
{
  http_test_server srv;
  srv.response = make_response(200, "OK", "hello world");
  srv.start();

  auto res = do_request(srv, "GET", "/test");

  REQUIRE(res.got_response);
  REQUIRE(!res.got_error);
  REQUIRE(res.status == 200);
  REQUIRE(res.body == "hello world");

  // Verify the server received a proper GET request
  REQUIRE(srv.received_request.find("GET /test HTTP/1.1") != std::string::npos);
  REQUIRE(srv.received_request.find("Host: 127.0.0.1") != std::string::npos);
  srv.stop();
}

TEST_CASE("http_client_get_with_path_and_query", "http_client_get_with_path_and_query")
{
  http_test_server srv;
  srv.response = make_response(200, "OK", "ok");
  srv.start();

  auto res = do_request(srv, "GET", "/api/search?q=hello&limit=10");

  REQUIRE(res.got_response);
  REQUIRE(res.status == 200);
  REQUIRE(srv.received_request.find("GET /api/search?q=hello&limit=10 HTTP/1.1")
          != std::string::npos);
  srv.stop();
}

// ---- Status codes ----

TEST_CASE("http_client_status_201", "http_client_status_201")
{
  http_test_server srv;
  srv.response = make_response(201, "Created", R"({"id":42})");
  srv.start();

  auto res = do_request(srv, "POST", "/resource");
  REQUIRE(res.got_response);
  REQUIRE(res.status == 201);
  REQUIRE(res.body == R"({"id":42})");
  srv.stop();
}

TEST_CASE("http_client_status_204", "http_client_status_204")
{
  http_test_server srv;
  srv.response = make_response(204, "No Content");
  srv.start();

  auto res = do_request(srv, "DELETE", "/resource/1");
  REQUIRE(res.got_response);
  REQUIRE(res.status == 204);
  REQUIRE(res.body.empty());
  srv.stop();
}

TEST_CASE("http_client_status_400", "http_client_status_400")
{
  http_test_server srv;
  srv.response = make_response(400, "Bad Request", "invalid input");
  srv.start();

  auto res = do_request(srv, "POST", "/data");
  REQUIRE(res.got_response);
  REQUIRE(res.status == 400);
  REQUIRE(res.body == "invalid input");
  srv.stop();
}

TEST_CASE("http_client_status_401", "http_client_status_401")
{
  http_test_server srv;
  srv.response = make_response(401, "Unauthorized", "not authenticated");
  srv.start();

  auto res = do_request(srv, "GET", "/protected");
  REQUIRE(res.got_response);
  REQUIRE(res.status == 401);
  srv.stop();
}

TEST_CASE("http_client_status_404", "http_client_status_404")
{
  http_test_server srv;
  srv.response = make_response(404, "Not Found", "no such resource");
  srv.start();

  auto res = do_request(srv, "GET", "/nonexistent");
  REQUIRE(res.got_response);
  REQUIRE(res.status == 404);
  srv.stop();
}

TEST_CASE("http_client_status_500", "http_client_status_500")
{
  http_test_server srv;
  srv.response = make_response(500, "Internal Server Error", "oops");
  srv.start();

  auto res = do_request(srv, "GET", "/broken");
  REQUIRE(res.got_response);
  REQUIRE(res.status == 500);
  srv.stop();
}

// ---- POST with JSON body ----

TEST_CASE("http_client_post_json", "http_client_post_json")
{
  http_test_server srv;
  srv.response = make_response(200, "OK", R"({"status":"ok"})");
  srv.start();

  std::string json_body = R"({"action":"start","num":1})";

  auto res = do_request(
      srv, "POST", "/api/timeline",
      {{"Content-Type", "application/json"}},
      json_body);

  REQUIRE(res.got_response);
  REQUIRE(res.status == 200);
  REQUIRE(res.body == R"({"status":"ok"})");

  // Verify server received correct verb, headers, and body
  REQUIRE(srv.received_request.find("POST /api/timeline HTTP/1.1") != std::string::npos);
  REQUIRE(srv.received_request.find("Content-Type: application/json") != std::string::npos);
  REQUIRE(srv.received_request.find("Content-Length: " + std::to_string(json_body.size()))
          != std::string::npos);
  REQUIRE(srv.received_request.find(json_body) != std::string::npos);
  srv.stop();
}

// ---- PUT ----

TEST_CASE("http_client_put", "http_client_put")
{
  http_test_server srv;
  srv.response = make_response(200, "OK", "updated");
  srv.start();

  auto res = do_request(
      srv, "PUT", "/api/resource/42",
      {{"Content-Type", "application/json"}},
      R"({"name":"new"})");

  REQUIRE(res.got_response);
  REQUIRE(res.status == 200);
  REQUIRE(srv.received_request.find("PUT /api/resource/42 HTTP/1.1") != std::string::npos);
  REQUIRE(srv.received_request.find(R"({"name":"new"})") != std::string::npos);
  srv.stop();
}

// ---- DELETE ----

TEST_CASE("http_client_delete", "http_client_delete")
{
  http_test_server srv;
  srv.response = make_response(204, "No Content");
  srv.start();

  auto res = do_request(srv, "DELETE", "/api/resource/42");

  REQUIRE(res.got_response);
  REQUIRE(res.status == 204);
  REQUIRE(srv.received_request.find("DELETE /api/resource/42 HTTP/1.1")
          != std::string::npos);
  srv.stop();
}

// ---- PATCH ----

TEST_CASE("http_client_patch", "http_client_patch")
{
  http_test_server srv;
  srv.response = make_response(200, "OK", "patched");
  srv.start();

  auto res = do_request(
      srv, "PATCH", "/api/item/7",
      {{"Content-Type", "application/json"}},
      R"({"level":0.5})");

  REQUIRE(res.got_response);
  REQUIRE(res.status == 200);
  REQUIRE(srv.received_request.find("PATCH /api/item/7 HTTP/1.1") != std::string::npos);
  srv.stop();
}

// ---- HEAD ----

TEST_CASE("http_client_head", "http_client_head")
{
  http_test_server srv;
  // HEAD response has Content-Length but no body
  srv.response
      = "HTTP/1.1 200 OK\r\n"
        "Content-Length: 42\r\n"
        "Connection: close\r\n"
        "\r\n";
  srv.start();

  auto res = do_request(srv, "HEAD", "/check");

  REQUIRE(res.got_response);
  REQUIRE(res.status == 200);
  REQUIRE(srv.received_request.find("HEAD /check HTTP/1.1") != std::string::npos);
  srv.stop();
}

// ---- Custom headers ----

TEST_CASE("http_client_bearer_auth", "http_client_bearer_auth")
{
  http_test_server srv;
  srv.response = make_response(200, "OK", "secret data");
  srv.start();

  auto res = do_request(
      srv, "GET", "/protected",
      {{"Authorization", "Bearer my-token-123"},
       {"Accept", "application/json"}});

  REQUIRE(res.got_response);
  REQUIRE(res.status == 200);
  REQUIRE(res.body == "secret data");
  REQUIRE(srv.received_request.find("Authorization: Bearer my-token-123")
          != std::string::npos);
  REQUIRE(srv.received_request.find("Accept: application/json") != std::string::npos);
  // User-supplied Accept should replace the default
  // Check there's no duplicate "Accept: */*"
  auto first_accept = srv.received_request.find("Accept:");
  auto second_accept = srv.received_request.find("Accept:", first_accept + 1);
  REQUIRE(second_accept == std::string::npos);
  srv.stop();
}

TEST_CASE("http_client_custom_content_type", "http_client_custom_content_type")
{
  http_test_server srv;
  srv.response = make_response(200, "OK", "ok");
  srv.start();

  auto res = do_request(
      srv, "POST", "/form",
      {{"Content-Type", "application/x-www-form-urlencoded"}},
      "grant_type=client_credentials&client_id=app");

  REQUIRE(res.got_response);
  REQUIRE(srv.received_request.find("Content-Type: application/x-www-form-urlencoded")
          != std::string::npos);
  REQUIRE(srv.received_request.find("grant_type=client_credentials") != std::string::npos);
  srv.stop();
}

TEST_CASE("http_client_multiple_custom_headers", "http_client_multiple_custom_headers")
{
  http_test_server srv;
  srv.response = make_response(200, "OK", "ok");
  srv.start();

  auto res = do_request(
      srv, "GET", "/api",
      {{"X-Api-Key", "abc123"},
       {"X-Request-Id", "req-456"},
       {"Accept-Language", "en-US"}});

  REQUIRE(res.got_response);
  REQUIRE(srv.received_request.find("X-Api-Key: abc123") != std::string::npos);
  REQUIRE(srv.received_request.find("X-Request-Id: req-456") != std::string::npos);
  REQUIRE(srv.received_request.find("Accept-Language: en-US") != std::string::npos);
  srv.stop();
}

// ---- Default headers ----

TEST_CASE("http_client_default_headers", "http_client_default_headers")
{
  http_test_server srv;
  srv.response = make_response(200, "OK", "ok");
  srv.start();

  // No custom headers — defaults should be present
  auto res = do_request(srv, "GET", "/");

  REQUIRE(res.got_response);
  REQUIRE(srv.received_request.find("Accept: */*") != std::string::npos);
  REQUIRE(srv.received_request.find("Connection: close") != std::string::npos);
  REQUIRE(srv.received_request.find("Host: 127.0.0.1") != std::string::npos);
  srv.stop();
}

TEST_CASE("http_client_auto_content_length", "http_client_auto_content_length")
{
  http_test_server srv;
  srv.response = make_response(200, "OK", "ok");
  srv.start();

  std::string body = "12345";
  auto res = do_request(srv, "POST", "/", {}, body);

  REQUIRE(res.got_response);
  REQUIRE(srv.received_request.find("Content-Length: 5") != std::string::npos);
  srv.stop();
}

// ---- Large body ----

TEST_CASE("http_client_large_response", "http_client_large_response")
{
  http_test_server srv;
  std::string large_body(100000, 'X');
  srv.response = make_response(200, "OK", large_body);
  srv.start();

  auto res = do_request(srv, "GET", "/large");

  REQUIRE(res.got_response);
  REQUIRE(res.status == 200);
  REQUIRE(res.body.size() == 100000);
  REQUIRE(res.body == large_body);
  srv.stop();
}

TEST_CASE("http_client_large_post", "http_client_large_post")
{
  http_test_server srv;
  srv.response = make_response(200, "OK", "received");
  srv.start();

  std::string large_body(50000, 'A');
  auto res = do_request(
      srv, "POST", "/upload",
      {{"Content-Type", "application/octet-stream"}},
      large_body);

  REQUIRE(res.got_response);
  REQUIRE(res.status == 200);
  REQUIRE(srv.received_request.find(large_body) != std::string::npos);
  srv.stop();
}

// ---- Empty body responses ----

TEST_CASE("http_client_empty_body", "http_client_empty_body")
{
  http_test_server srv;
  srv.response = make_response(200, "OK", "");
  srv.start();

  auto res = do_request(srv, "GET", "/empty");

  REQUIRE(res.got_response);
  REQUIRE(res.status == 200);
  REQUIRE(res.body.empty());
  srv.stop();
}

// ---- No Content-Length (read until EOF) ----

TEST_CASE("http_client_no_content_length", "http_client_no_content_length")
{
  http_test_server srv;
  // Response without Content-Length — client reads until EOF
  srv.response
      = "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "\r\n"
        "streamed data here";
  srv.start();

  auto res = do_request(srv, "GET", "/stream");

  REQUIRE(res.got_response);
  REQUIRE(res.status == 200);
  REQUIRE(res.body == "streamed data here");
  srv.stop();
}

// ---- Connection refused ----

TEST_CASE("http_client_connection_refused", "http_client_connection_refused")
{
  client_result res;
  std::atomic_bool finished{false};

  asio::io_context client_ctx;

  // Connect to a port where nothing is listening
  auto req = std::make_shared<test_request>(
      test_answer{&res, &finished}, test_error{&res, &finished}, client_ctx, "GET",
      "127.0.0.1", "/",
      std::vector<std::pair<std::string, std::string>>{}, "");

  req->resolve("127.0.0.1", "19999");

  client_ctx.run();

  REQUIRE(res.got_error);
  REQUIRE(!res.got_response);
  REQUIRE(!res.error.empty());
}

// ---- Path with spaces (URL encoding) ----

TEST_CASE("http_client_path_space_encoding", "http_client_path_space_encoding")
{
  http_test_server srv;
  srv.response = make_response(200, "OK", "ok");
  srv.start();

  auto res = do_request(srv, "GET", "/path with spaces/file name");

  REQUIRE(res.got_response);
  REQUIRE(srv.received_request.find("GET /path%20with%20spaces/file%20name HTTP/1.1")
          != std::string::npos);
  srv.stop();
}

// ---- No body on GET (no Content-Length header sent) ----

TEST_CASE("http_client_get_no_body_header", "http_client_get_no_body_header")
{
  http_test_server srv;
  srv.response = make_response(200, "OK", "ok");
  srv.start();

  auto res = do_request(srv, "GET", "/");

  REQUIRE(res.got_response);
  // GET without body should NOT have Content-Length or Content-Type
  REQUIRE(srv.received_request.find("Content-Length") == std::string::npos);
  REQUIRE(srv.received_request.find("Content-Type") == std::string::npos);
  srv.stop();
}

// ---- POST with empty body ----

TEST_CASE("http_client_post_empty_body", "http_client_post_empty_body")
{
  http_test_server srv;
  srv.response = make_response(200, "OK", "ok");
  srv.start();

  auto res = do_request(srv, "POST", "/empty-post", {}, "");

  REQUIRE(res.got_response);
  REQUIRE(res.status == 200);
  // Empty body: no Content-Length should be added
  REQUIRE(srv.received_request.find("POST /empty-post HTTP/1.1") != std::string::npos);
  srv.stop();
}
