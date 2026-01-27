#ifndef RIDER_H
#define RIDER_H

#include <string>
using namespace std;

class Rider {
private:
    int id;
    string name;

public:
    Rider();
    Rider(int id, const string& name);

    // Getters
    int getId() const;
    string getName() const;

    // Display
    void display() const;
};

#endif
