#ifndef HV_WEBSOCKET_SERVER_H_
#define HV_WEBSOCKET_SERVER_H_

/*
 * @demo examples/websocket_server_test.cpp
 */

#include "common/include/libnet/http/server/HttpServer.h"
#include "common/include/libnet/http/WebSocketChannel.h"

#define websocket_server_t      http_server_t
#define websocket_server_run    http_server_run
#define websocket_server_stop   http_server_stop

namespace hv {

struct WebSocketService {
    std::function<void(const WebSocketChannelPtr&, const HttpRequestPtr&)>  onopen;
    std::function<void(const WebSocketChannelPtr&, const std::string&)>     onmessage;
    std::function<void(const WebSocketChannelPtr&)>                         onclose;
    int ping_interval;

    WebSocketService() : ping_interval(0) {}

    void setPingInterval(int ms) {
        ping_interval = ms;
    }
};

class WebSocketServer : public HttpServer {
public:
    WebSocketServer(WebSocketService* service = NULL)
        : HttpServer()
    {
        this->ws = service;
    }
    ~WebSocketServer() { stop(); }

    void registerWebSocketService(WebSocketService* service) {
        this->ws = service;
    }
};

}

#endif // HV_WEBSOCKET_SERVER_H_
