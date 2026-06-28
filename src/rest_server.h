#pragma once

#include "api_interface.h"
#include "httplib.h"
#include <chrono>
#include <thread>
#include <memory>

// Start time
extern std::chrono::steady_clock::time_point g_start_time;

// RestServer implementation
class RestServer : public api::IApiHandler, public std::enable_shared_from_this<RestServer> {
public:
    RestServer();
    ~RestServer();

    void start(int port = 8080);
    void stop();
    std::string get_url() const;

    api::ApiHelloResponseDto get_api_hello() override;
    api::ApiDataResponseDto get_api_data() override;
    api::ApiEchoResponseDto post_api_echo(const api::ApiEchoRequestDto& req) override;
    api::ApiStatusResponseDto get_api_status() override;

private:
    void setup_common_routes(httplib::Server& server);

    std::unique_ptr<httplib::Server> server_;
    std::unique_ptr<std::thread> server_thread_;
    int port_;
};
