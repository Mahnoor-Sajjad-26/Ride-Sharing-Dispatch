// Fix for Windows SDK byte conflict with C++17
#define _HAS_STD_BYTE 0

#include "WebServer.h"
#include <iostream>
#include <sstream>

using namespace std;

WebServer::WebServer(RideShareSystem* sys, int port) : system(sys), port(port) {
    setupRoutes();
}

WebServer::~WebServer() {
    stop();
}

string WebServer::createJSONResponse(bool success, const string& message, const string& data) {
    ostringstream json;
    json << "{";
    json << "\"success\": " << (success ? "true" : "false") << ", ";
    json << "\"message\": \"" << message << "\"";
    if (!data.empty()) {
        json << ", \"data\": " << data;
    }
    json << "}";
    return json.str();
}

string WebServer::locationsToJSON() {
    ostringstream json;
    json << "[";

    City& city = system->getCity();
    int locationCount = city.getLocationCount();

    for (int i = 0; i < locationCount; i++) {
        Location* loc = city.getLocation(i);
        if (loc) {
            if (i > 0) json << ", ";
            json << "{";
            json << "\"id\": " << loc->id << ", ";
            json << "\"name\": \"" << loc->name << "\", ";
            json << "\"zone\": " << loc->zoneId;
            json << "}";
        }
    }

    json << "]";
    return json.str();
}

string WebServer::roadsToJSON() {
    ostringstream json;
    json << "[";

    City& city = system->getCity();
    int locationCount = city.getLocationCount();
    bool first = true;

    for (int i = 0; i < locationCount; i++) {
        Location* loc = city.getLocation(i);
        if (loc) {
            Edge* edge = loc->adjacencyList;
            while (edge) {
                // Only export each edge once (avoid duplicates for undirected graph)
                if (i < edge->destination) {
                    if (!first) json << ", ";
                    json << "{";
                    json << "\"from\": " << i << ", ";
                    json << "\"to\": " << edge->destination << ", ";
                    json << "\"distance\": " << edge->weight;
                    json << "}";
                    first = false;
                }
                edge = edge->next;
            }
        }
    }

    json << "]";
    return json.str();
}

string WebServer::driversToJSON() {
    ostringstream json;
    json << "[";

    int driverCount = system->getDriverCount();
    for (int i = 0; i < driverCount; i++) {
        Driver* driver = system->getDriver(i);
        if (driver) {
            if (i > 0) json << ", ";
            json << "{";
            json << "\"id\": " << driver->getId() << ", ";
            json << "\"name\": \"" << driver->getName() << "\", ";
            json << "\"location\": " << driver->getCurrentLocationId() << ", ";
            json << "\"zone\": " << driver->getZoneId() << ", ";
            json << "\"status\": " << driver->getStatus() << ", ";
            json << "\"trips\": " << driver->getTotalTripsCompleted() << ", ";
            json << "\"distance\": " << driver->getTotalDistanceCovered();
            json << "}";
        }
    }

    json << "]";
    return json.str();
}

string WebServer::tripsToJSON() {
    ostringstream json;
    json << "[";

    int tripCount = system->getTotalTripCount();
    for (int i = 0; i < tripCount; i++) {
        Trip* trip = system->getTrip(i);
        if (trip) {
            if (i > 0) json << ", ";
            json << "{";
            json << "\"id\": " << trip->getId() << ", ";
            json << "\"riderId\": " << trip->getRiderId() << ", ";
            json << "\"driverId\": " << trip->getDriverId() << ", ";
            json << "\"pickup\": " << trip->getPickupLocationId() << ", ";
            json << "\"dropoff\": " << trip->getDropoffLocationId() << ", ";
            json << "\"state\": " << trip->getState() << ", ";
            json << "\"distance\": " << trip->getDistance();
            json << "}";
        }
    }

    json << "]";
    return json.str();
}

string WebServer::analyticsToJSON() {
    ostringstream json;
    json << "{";
    json << "\"totalTrips\": " << system->getTotalTripCount() << ", ";
    json << "\"completedTrips\": " << system->getCompletedTripCount() << ", ";
    json << "\"cancelledTrips\": " << system->getCancelledTripCount() << ", ";
    json << "\"avgTripDistance\": " << system->getAverageTripDistance();
    json << "}";
    return json.str();
}

string WebServer::systemStateToJSON() {
    ostringstream json;
    json << "{";
    json << "\"locations\": " << locationsToJSON() << ", ";
    json << "\"roads\": " << roadsToJSON() << ", ";
    json << "\"drivers\": " << driversToJSON() << ", ";
    json << "\"trips\": " << tripsToJSON() << ", ";
    json << "\"analytics\": " << analyticsToJSON();
    json << "}";
    return json.str();
}

void WebServer::setupRoutes() {
    // Enable CORS for all routes
    server.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    });

    // GET /api/state - Get complete system state
    server.Get("/api/state", [this](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(systemStateToJSON(), "application/json");
    });

    // GET /api/drivers - Get all drivers
    server.Get("/api/drivers", [this](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(driversToJSON(), "application/json");
    });

    // GET /api/trips - Get all trips
    server.Get("/api/trips", [this](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(tripsToJSON(), "application/json");
    });

    // GET /api/analytics - Get analytics
    server.Get("/api/analytics", [this](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(analyticsToJSON(), "application/json");
    });

    // POST /api/trips/request - Request a new trip
    server.Post("/api/trips/request", [this](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json");

        // Parse parameters
        int riderId = -1, pickup = -1, dropoff = -1;

        if (req.has_param("riderId")) riderId = stoi(req.get_param_value("riderId"));
        if (req.has_param("pickup")) pickup = stoi(req.get_param_value("pickup"));
        if (req.has_param("dropoff")) dropoff = stoi(req.get_param_value("dropoff"));

        if (riderId < 0 || pickup < 0 || dropoff < 0) {
            res.set_content(createJSONResponse(false, "Invalid parameters"), "application/json");
            return;
        }

        int tripId = system->requestTrip(riderId, pickup, dropoff);
        if (tripId >= 0) {
            // Auto-assign driver
            bool assigned = system->assignTrip(tripId);
            ostringstream data;
            data << "{\"tripId\": " << tripId << ", \"assigned\": " << (assigned ? "true" : "false") << "}";
            res.set_content(createJSONResponse(true, "Trip requested and assigned", data.str()), "application/json");
        } else {
            res.set_content(createJSONResponse(false, "Failed to request trip"), "application/json");
        }
    });

    // POST /api/trips/start - Start a trip
    server.Post("/api/trips/start", [this](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json");

        if (!req.has_param("tripId")) {
            res.set_content(createJSONResponse(false, "Missing tripId parameter"), "application/json");
            return;
        }

        int tripId = stoi(req.get_param_value("tripId"));
        bool success = system->startTrip(tripId);

        if (success) {
            res.set_content(createJSONResponse(true, "Trip started successfully"), "application/json");
        } else {
            res.set_content(createJSONResponse(false, "Failed to start trip"), "application/json");
        }
    });

    // POST /api/trips/complete - Complete a trip
    server.Post("/api/trips/complete", [this](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json");

        if (!req.has_param("tripId")) {
            res.set_content(createJSONResponse(false, "Missing tripId parameter"), "application/json");
            return;
        }

        int tripId = stoi(req.get_param_value("tripId"));
        bool success = system->completeTrip(tripId);

        if (success) {
            res.set_content(createJSONResponse(true, "Trip completed successfully"), "application/json");
        } else {
            res.set_content(createJSONResponse(false, "Failed to complete trip"), "application/json");
        }
    });

    // POST /api/trips/cancel - Cancel a trip
    server.Post("/api/trips/cancel", [this](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json");

        if (!req.has_param("tripId")) {
            res.set_content(createJSONResponse(false, "Missing tripId parameter"), "application/json");
            return;
        }

        int tripId = stoi(req.get_param_value("tripId"));
        bool success = system->cancelTrip(tripId);

        if (success) {
            res.set_content(createJSONResponse(true, "Trip cancelled successfully"), "application/json");
        } else {
            res.set_content(createJSONResponse(false, "Failed to cancel trip"), "application/json");
        }
    });

    // POST /api/rollback - Rollback operations
    server.Post("/api/rollback", [this](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json");

        int count = 1;
        if (req.has_param("count")) {
            count = stoi(req.get_param_value("count"));
        }

        bool success = system->rollback(count);

        if (success) {
            ostringstream data;
            data << "{\"rolledBack\": " << count << "}";
            res.set_content(createJSONResponse(true, "Rollback successful", data.str()), "application/json");
        } else {
            res.set_content(createJSONResponse(false, "Rollback failed"), "application/json");
        }
    });

    // Serve the dashboard HTML file
    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_redirect("/dashboard.html");
    });

    // Serve static files (dashboard.html)
    server.set_mount_point("/", ".");
}

void WebServer::start() {
    cout << "\n========================================" << endl;
    cout << "  WEB SERVER STARTING" << endl;
    cout << "========================================" << endl;
    cout << "Server running at: http://localhost:" << port << endl;
    cout << "Dashboard: http://localhost:" << port << "/dashboard.html" << endl;
    cout << "API Endpoint: http://localhost:" << port << "/api/state" << endl;
    cout << "\nPress Ctrl+C to stop the server..." << endl;
    cout << "========================================\n" << endl;

    server.listen("0.0.0.0", port);
}

void WebServer::stop() {
    server.stop();
}
