#include "Vector2d.h"

Vector2d::Vector2d() {
    x = 0.0;
    y = 0.0;
}
Vector2d::Vector2d(double xx, double yy) : x(xx), y(yy) {}

Vector2d::~Vector2d() {}

Vector2d Vector2d::operator+(const Vector2d& other) const {
    return Vector2d(x + other.x, y + other.y);
}

void Vector2d::operator+=(const Vector2d& other) {
    x += other.x;
    y += other.y;
}

Vector2d Vector2d::operator-(const Vector2d& other) const {
    return Vector2d(x - other.x, y - other.y);
}

double Vector2d::operator*(const Vector2d& other) const {
    return x * other.x + y * other.y;
}

Vector2d Vector2d::operator*(const double scalar) const {
    return Vector2d(x * scalar, y * scalar);
}

double Vector2d::length() {
    return sqrt(x * x + y * y);
}

void Vector2d::normalize() {
    double len = length();
    if(len == 0) return;

    x /= len;
    y /= len;
}

void Vector2d::rotate(double angle) {
    double x_new = cos(angle) * x - sin(angle) * y;
    y = sin(angle) * x + cos(angle) * y;
    x = x_new;
}
