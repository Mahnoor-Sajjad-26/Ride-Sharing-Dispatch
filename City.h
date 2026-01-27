#ifndef CITY_H
#define CITY_H

#include <string>
using namespace std;

// Edge in adjacency list (linked list node)
struct Edge {
    int destination;
    int weight;
    Edge* next;

    Edge(int dest, int w);
};

// Location/Node in the graph
struct Location {
    int id;
    string name;
    int zoneId;
    Edge* adjacencyList;

    Location();
    Location(int id, const string& name, int zone);
    ~Location();
    void addEdge(int destination, int weight);
};

// City graph class
class City {
private:
    Location* locations;
    int locationCount;
    int capacity;

    void resize();
    int findMinDistance(int* dist, bool* visited, int n);

public:
    City(int initialCapacity = 10);
    ~City();

    // Graph operations
    int addLocation(const string& name, int zoneId);
    void addRoad(int from, int to, int distance);

    // Shortest path using Dijkstra's algorithm
    int getDistance(int source, int destination);
    int* getShortestPath(int source, int destination, int& pathLength);

    // Getters
    int getLocationCount() const;
    Location* getLocation(int id);
    string getLocationName(int id) const;
    int getLocationZone(int id) const;

    // Display
    void displayCity() const;
};

#endif
