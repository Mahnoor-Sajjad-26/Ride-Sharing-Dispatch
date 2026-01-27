#include "City.h"
#include <iostream>
#include <climits>
using namespace std;

// Edge constructor
Edge::Edge(int dest, int w) : destination(dest), weight(w), next(nullptr) {}

// Location constructors
Location::Location() : id(-1), name(""), zoneId(-1), adjacencyList(nullptr) {}

Location::Location(int id, const string& name, int zone)
    : id(id), name(name), zoneId(zone), adjacencyList(nullptr) {}

Location::~Location() {
    Edge* current = adjacencyList;
    while (current != nullptr) {
        Edge* next = current->next;
        delete current;
        current = next;
    }
}

void Location::addEdge(int destination, int weight) {
    Edge* newEdge = new Edge(destination, weight);
    newEdge->next = adjacencyList;
    adjacencyList = newEdge;
}

// City constructor
City::City(int initialCapacity) : locationCount(0), capacity(initialCapacity) {
    locations = new Location[capacity];
}

City::~City() {
    delete[] locations;
}

void City::resize() {
    int newCapacity = capacity * 2;
    Location* newLocations = new Location[newCapacity];

    for (int i = 0; i < locationCount; i++) {
        newLocations[i].id = locations[i].id;
        newLocations[i].name = locations[i].name;
        newLocations[i].zoneId = locations[i].zoneId;
        newLocations[i].adjacencyList = locations[i].adjacencyList;
        locations[i].adjacencyList = nullptr; // Prevent double delete
    }

    delete[] locations;
    locations = newLocations;
    capacity = newCapacity;
}

int City::addLocation(const string& name, int zoneId) {
    if (locationCount >= capacity) {
        resize();
    }

    int id = locationCount;
    locations[locationCount] = Location(id, name, zoneId);
    locationCount++;
    return id;
}

void City::addRoad(int from, int to, int distance) {
    if (from < 0 || from >= locationCount || to < 0 || to >= locationCount) {
        cout << "Invalid location IDs for road." << endl;
        return;
    }

    // Add bidirectional road
    locations[from].addEdge(to, distance);
    locations[to].addEdge(from, distance);
}

int City::findMinDistance(int* dist, bool* visited, int n) {
    int minDist = INT_MAX;
    int minIndex = -1;

    for (int i = 0; i < n; i++) {
        if (!visited[i] && dist[i] < minDist) {
            minDist = dist[i];
            minIndex = i;
        }
    }
    return minIndex;
}

int City::getDistance(int source, int destination) {
    if (source < 0 || source >= locationCount ||
        destination < 0 || destination >= locationCount) {
        return -1;
    }

    if (source == destination) {
        return 0;
    }

    // Dijkstra's algorithm
    int* dist = new int[locationCount];
    bool* visited = new bool[locationCount];

    for (int i = 0; i < locationCount; i++) {
        dist[i] = INT_MAX;
        visited[i] = false;
    }

    dist[source] = 0;

    for (int count = 0; count < locationCount - 1; count++) {
        int u = findMinDistance(dist, visited, locationCount);
        if (u == -1) break;

        visited[u] = true;

        // Update distances for adjacent vertices
        Edge* edge = locations[u].adjacencyList;
        while (edge != nullptr) {
            int v = edge->destination;
            if (!visited[v] && dist[u] != INT_MAX &&
                dist[u] + edge->weight < dist[v]) {
                dist[v] = dist[u] + edge->weight;
            }
            edge = edge->next;
        }
    }

    int result = (dist[destination] == INT_MAX) ? -1 : dist[destination];

    delete[] dist;
    delete[] visited;

    return result;
}

int* City::getShortestPath(int source, int destination, int& pathLength) {
    if (source < 0 || source >= locationCount ||
        destination < 0 || destination >= locationCount) {
        pathLength = 0;
        return nullptr;
    }

    // Dijkstra's with path tracking
    int* dist = new int[locationCount];
    int* parent = new int[locationCount];
    bool* visited = new bool[locationCount];

    for (int i = 0; i < locationCount; i++) {
        dist[i] = INT_MAX;
        parent[i] = -1;
        visited[i] = false;
    }

    dist[source] = 0;

    for (int count = 0; count < locationCount - 1; count++) {
        int u = findMinDistance(dist, visited, locationCount);
        if (u == -1) break;

        visited[u] = true;

        Edge* edge = locations[u].adjacencyList;
        while (edge != nullptr) {
            int v = edge->destination;
            if (!visited[v] && dist[u] != INT_MAX &&
                dist[u] + edge->weight < dist[v]) {
                dist[v] = dist[u] + edge->weight;
                parent[v] = u;
            }
            edge = edge->next;
        }
    }

    // Reconstruct path
    if (dist[destination] == INT_MAX) {
        pathLength = 0;
        delete[] dist;
        delete[] parent;
        delete[] visited;
        return nullptr;
    }

    // Count path length
    pathLength = 0;
    int current = destination;
    while (current != -1) {
        pathLength++;
        current = parent[current];
    }

    // Build path array (reverse order)
    int* path = new int[pathLength];
    current = destination;
    for (int i = pathLength - 1; i >= 0; i--) {
        path[i] = current;
        current = parent[current];
    }

    delete[] dist;
    delete[] parent;
    delete[] visited;

    return path;
}

int City::getLocationCount() const {
    return locationCount;
}

Location* City::getLocation(int id) {
    if (id < 0 || id >= locationCount) {
        return nullptr;
    }
    return &locations[id];
}

string City::getLocationName(int id) const {
    if (id < 0 || id >= locationCount) {
        return "";
    }
    return locations[id].name;
}

int City::getLocationZone(int id) const {
    if (id < 0 || id >= locationCount) {
        return -1;
    }
    return locations[id].zoneId;
}

void City::displayCity() const {
    cout << "\n=== City Map ===" << endl;
    for (int i = 0; i < locationCount; i++) {
        cout << "Location " << i << " (" << locations[i].name
             << ") [Zone " << locations[i].zoneId << "]: ";

        Edge* edge = locations[i].adjacencyList;
        while (edge != nullptr) {
            cout << "-> " << locations[edge->destination].name
                 << "(" << edge->weight << ") ";
            edge = edge->next;
        }
        cout << endl;
    }
}
