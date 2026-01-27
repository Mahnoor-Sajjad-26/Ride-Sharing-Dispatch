#include "Rider.h"
#include <iostream>
using namespace std;

Rider::Rider() : id(-1), name("") {}

Rider::Rider(int id, const string& name) : id(id), name(name) {}

int Rider::getId() const {
    return id;
}

string Rider::getName() const {
    return name;
}

void Rider::display() const {
    cout << "Rider " << id << ": " << name << endl;
}
