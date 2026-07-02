#include "rest_server.h"
#include "api_server.h"
#include <string>
#include <ctime>
#include <fstream>
#include <sstream>
#include <filesystem>

// Start time
std::chrono::steady_clock::time_point g_start_time = std::chrono::steady_clock::now();

RestServer::RestServer() : port_(8080) {}

RestServer::~RestServer() {
    stop();
}

void RestServer::start(int port) {
    port_ = port;
    
    server_ = std::make_unique<httplib::Server>();
    api_server_ = std::make_unique<api::ApiServer>(this);
    
    // Setup API routes
    api_server_->setup_routes(*server_);
    
    // Setup common routes
    setup_common_routes(*server_);
    
    // Run server in background thread
    server_thread_ = std::make_unique<std::thread>([this]() {
        server_->listen("127.0.0.1", port_);
    });
    
    // Wait for server to start
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void RestServer::stop() {
    if (server_) {
        server_->stop();
    }
    if (server_thread_ && server_thread_->joinable()) {
        server_thread_->join();
    }
}

std::string RestServer::get_url() const {
    return "http://127.0.0.1:" + std::to_string(port_);
}

std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void RestServer::setup_common_routes(httplib::Server& server) {
    // Add CORS support
    server.set_pre_routing_handler([](const auto&, auto& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        return httplib::Server::HandlerResponse::Unhandled;
    });

    // Handle OPTIONS requests
    server.Options("/api/.*", [](const auto&, auto& res) {
        res.status = 200;
    });

    // Add HTML page endpoint - reads from ui/index.html file
    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        std::string html_content = read_file("ui/index.html");
        if (html_content.empty()) {
            res.status = 500;
            res.set_content("Could not load UI file", "text/plain");
        } else {
            res.set_content(html_content, "text/html; charset=utf-8");
        }
    });
}

api::ApiHelloResponseDto RestServer::get_api_hello() {
    return { "Hello, World!" };
}

api::ApiDataResponseDto RestServer::get_api_data() {
    return {
        { "Apple", "Banana", "Cherry", "Date" },
        4
    };
}

api::ApiEchoResponseDto RestServer::post_api_echo(const api::ApiEchoRequestDto& req) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::string timestamp = std::ctime(&time_t);
    timestamp.pop_back(); // Remove newline

    return { req.message, timestamp };
}

api::ApiStatusResponseDto RestServer::get_api_status() {
    auto now = std::chrono::steady_clock::now();
    auto uptime = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(now - g_start_time).count());

    return { "running", uptime, "1.0.0" };
}
