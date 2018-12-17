#include "Pos.hpp"

Pos::Pos() : x(0.0f), y(0.0f), z(0.0f) {

}

Pos::Pos(float x, float y, float z) : x(x), y(y), z(z) {

}

Pos::Pos(const Pos &pos) {
    this->x = pos.getX();
    this->y = pos.getY();
    this->z = pos.getZ();
}

Pos::~Pos() = default;

void Pos::setX(float x) {
    this->x = x;
}

void Pos::setY(float y) {
    this->y = y;
}

void Pos::setZ(float z) {
    this->z = z;
}

float Pos::getX() const {
    return x;
}

float Pos::getY() const {
    return y;
}

float Pos::getZ() const {
    return z;
}