#include "rest_server.h"
#include "api_server.h"
#include <string>
#include <ctime>

// Start time
std::chrono::steady_clock::time_point g_start_time = std::chrono::steady_clock::now();

RestServer::RestServer() : port_(8080) {}

RestServer::~RestServer() {
    stop();
}

void RestServer::start(int port) {
    port_ = port;
    
    server_ = std::make_unique<httplib::Server>();
    
    // Setup API routes
    api::ApiServer api_server(shared_from_this());
    api_server.setup_routes(*server_);
    
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

    // Add HTML page endpoint
    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(R"html(<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>WVUI API Tester</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            flex-direction: column;
            align-items: center;
            min-height: 100vh;
            margin: 0;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            padding: 20px;
        }
        .container {
            text-align: center;
            color: white;
            padding: 40px;
            max-width: 1000px;
            width: 100%;
        }
        h1 {
            font-size: 3em;
            margin-bottom: 10px;
        }
        .subtitle {
            font-size: 1.2em;
            opacity: 0.9;
            margin-bottom: 30px;
        }
        .api-section {
            background: rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 20px;
            margin: 10px 0;
            text-align: left;
        }
        .api-title {
            font-size: 1.2em;
            font-weight: bold;
            margin-bottom: 10px;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .method {
            background: #4CAF50;
            color: white;
            padding: 4px 8px;
            border-radius: 4px;
            font-size: 0.8em;
            font-weight: bold;
        }
        .method.get { background: #2196F3; }
        .method.post { background: #FF9800; }
        .api-description {
            font-size: 0.9em;
            opacity: 0.8;
            margin-bottom: 10px;
        }
        .api-status {
            padding: 4px 8px;
            border-radius: 4px;
            font-size: 0.8em;
            font-weight: bold;
        }
        .status-success { background: #4CAF50; }
        .status-error { background: #F44336; }
        .status-pending { background: #FF9800; }
        .api-response {
            background: rgba(0, 0, 0, 0.2);
            border-radius: 5px;
            padding: 10px;
            margin-top: 10px;
            font-family: monospace;
            font-size: 0.9em;
            white-space: pre-wrap;
            word-break: break-all;
            display: none;
        }
        .api-input {
            width: 100%;
            padding: 10px;
            margin: 10px 0;
            border-radius: 5px;
            border: none;
            font-family: monospace;
            background: rgba(0, 0, 0, 0.2);
            color: white;
        }
        button {
            background: white;
            color: #667eea;
            border: none;
            padding: 10px 20px;
            font-size: 1em;
            border-radius: 8px;
            cursor: pointer;
            transition: transform 0.2s;
        }
        button:hover {
            transform: scale(1.05);
        }
        .status-overview {
            background: rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 20px;
            text-align: left;
        }
        .status-grid {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
            gap: 10px;
            margin-top: 10px;
        }
        .status-item {
            padding: 8px;
            border-radius: 5px;
            background: rgba(0, 0, 0, 0.2);
            font-size: 0.9em;
        }
        .test-all-btn {
            background: #4CAF50;
            color: white;
            font-size: 1.1em;
            margin: 20px 0;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>WVUI API Tester</h1>
        <p class="subtitle">Test all REST endpoints with fake data</p>
        
        <div class="status-overview">
            <h2>API Status Overview</h2>
            <div class="status-grid" id="status-grid">
                <div class="status-item">
                    <strong>/api/hello:</strong>
                    <span id="status-hello" class="api-status status-pending">Pending</span>
                </div>
                <div class="status-item">
                    <strong>/api/data:</strong>
                    <span id="status-data" class="api-status status-pending">Pending</span>
                </div>
                <div class="status-item">
                    <strong>/api/echo:</strong>
                    <span id="status-echo" class="api-status status-pending">Pending</span>
                </div>
                <div class="status-item">
                    <strong>/api/status:</strong>
                    <span id="status-status" class="api-status status-pending">Pending</span>
                </div>
            </div>
            <button class="test-all-btn" onclick="testAllEndpoints()">Test All Endpoints</button>
        </div>
        
        <div class="api-section">
            <div class="api-title">
                <span><span class="method get">GET</span> /api/hello</span>
                <button onclick="testEndpoint('/api/hello', 'hello-response', 'status-hello')">Test</button>
            </div>
            <div class="api-description">Get a hello message from the server</div>
            <div id="hello-response" class="api-response"></div>
        </div>
        
        <div class="api-section">
            <div class="api-title">
                <span><span class="method get">GET</span> /api/data</span>
                <button onclick="testEndpoint('/api/data', 'data-response', 'status-data')">Test</button>
            </div>
            <div class="api-description">Get sample data from the server</div>
            <div id="data-response" class="api-response"></div>
        </div>
        
        <div class="api-section">
            <div class="api-title">
                <span><span class="method post">POST</span> /api/echo</span>
                <button onclick="testEcho()">Test</button>
            </div>
            <div class="api-description">Echo back the input data</div>
            <textarea id="echo-input" class="api-input" rows="4">{"message": "Hello from webview!"}</textarea>
            <div id="echo-response" class="api-response"></div>
        </div>
        
        <div class="api-section">
            <div class="api-title">
                <span><span class="method get">GET</span> /api/status</span>
                <button onclick="testEndpoint('/api/status', 'status-response', 'status-status')">Test</button>
            </div>
            <div class="api-description">Get server status</div>
            <div id="status-response" class="api-response"></div>
        </div>
    </div>
    <script>
        // Use relative path for API calls
        const API_BASE = '';
        
        function setStatus(statusId, success, message) {
            const statusEl = document.getElementById(statusId);
            statusEl.textContent = message;
            if (success === null) {
                statusEl.className = 'api-status status-pending';
            } else if (success) {
                statusEl.className = 'api-status status-success';
            } else {
                statusEl.className = 'api-status status-error';
            }
        }
        
        async function testEndpoint(path, responseId, statusId) {
            const responseDiv = document.getElementById(responseId);
            responseDiv.style.display = 'block';
            responseDiv.textContent = 'Loading...';
            setStatus(statusId, null, 'Testing...');
            
            try {
                const response = await fetch(API_BASE + path);
                const data = await response.json();
                responseDiv.textContent = JSON.stringify(data, null, 2);
                
                if (response.ok) {
                    setStatus(statusId, true, 'OK');
                } else {
                    setStatus(statusId, false, 'Failed');
                }
            } catch (error) {
                responseDiv.textContent = 'Error: ' + error.message;
                setStatus(statusId, false, 'Error');
            }
        }
        
        async function testEcho() {
            const input = document.getElementById('echo-input');
            const responseDiv = document.getElementById('echo-response');
            responseDiv.style.display = 'block';
            responseDiv.textContent = 'Loading...';
            setStatus('status-echo', null, 'Testing...');
            
            try {
                const response = await fetch(API_BASE + '/api/echo', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: input.value
                });
                const data = await response.json();
                responseDiv.textContent = JSON.stringify(data, null, 2);
                
                if (response.ok) {
                    setStatus('status-echo', true, 'OK');
                } else {
                    setStatus('status-echo', false, 'Failed');
                }
            } catch (error) {
                responseDiv.textContent = 'Error: ' + error.message;
                setStatus('status-echo', false, 'Error');
            }
        }
        
        async function testAllEndpoints() {
            // Reset statuses
            setStatus('status-hello', null, 'Testing...');
            setStatus('status-data', null, 'Testing...');
            setStatus('status-echo', null, 'Testing...');
            setStatus('status-status', null, 'Testing...');
            
            // Test all endpoints
            await Promise.all([
                testEndpoint('/api/hello', 'hello-response', 'status-hello'),
                testEndpoint('/api/data', 'data-response', 'status-data'),
                testEcho(),
                testEndpoint('/api/status', 'status-response', 'status-status')
            ]);
        }
    </script>
</body>
</html>
)html", "text/html; charset=utf-8");
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
