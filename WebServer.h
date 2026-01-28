#ifndef WEB_SERVER_H
#define WEB_SERVER_H

// Fix for Windows SDK byte conflict with C++17
#define _HAS_STD_BYTE 0

#include "RideShareSystem.h"
#include "httplib.h"
#include <string>

class WebServer {
private:
    RideShareSystem* system;
    httplib::Server server;
    int port;

    // Helper methods for JSON responses
    std::string createJSONResponse(bool success, const std::string& message, const std::string& data = "");
    std::string driversToJSON();
    std::string tripsToJSON();
    std::string analyticsToJSON();
    std::string systemStateToJSON();
    std::string locationsToJSON();
    std::string roadsToJSON();

    // Setup all API routes
    void setupRoutes();

public:
    WebServer(RideShareSystem* sys, int port = 8080);
    ~WebServer();

    void start();
    void stop();
};

#endif
