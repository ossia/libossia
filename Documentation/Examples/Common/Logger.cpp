#include <ossia/network/common/websocket_log_sink.hpp>

int main()
{
    auto connection = std::make_shared<ossia::websocket_threaded_connection>("ws://127.0.0.1:1337");
    auto log_sink = std::make_shared<ossia::websocket_log_sink>(connection, "name of the app");
    spdlog::logger logger("", log_sink);

    logger.info("hello, {} {}", "text", 123);
    /* This sends:
    {
        "operation": "log",
        "level": "info",
        "sender": "name of the app"
        "message": "hello text 123"
    }
    */

    logger.error("text: {} {}", "text", "blah");
    logger.debug("text: {} {}", "text", "blah");
    logger.critical("text: {} {}", "text", "blah");
    logger.warn("text: {} {}", "text", "blah");

    // Note: don't do
    logger.debug("my whole message");
    // instead do
    logger.debug("{}", "my whole message");
    // else there can be problems if there are { } in the log message.

    // The connection is shared between the log and the heartbeat.
    ossia::websocket_heartbeat heartbeat{
                connection,
                "the app name",
                std::chrono::seconds(2)};

    heartbeat.send_init({
                          {"pid", getpid()}
                        , {"cmd", "c:\\myapp\\app.exe"}
                        });

    // Now an "alive" message will be sent every 2 seconds as long as
    // the heartbeat is in scope.
    std::this_thread::sleep_for(std::chrono::seconds(6));
}
