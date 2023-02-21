#ifndef VECTOR2D_H
#define VECTOR2D_H
#include <cmath>

class Vector2d {
public:
    double x;
    double y;

    Vector2d();
    Vector2d(double xx, double yy);
    virtual ~Vector2d();

    Vector2d operator+(const Vector2d& other) const;

    void operator+=(const Vector2d& other);

    Vector2d operator-(const Vector2d& other) const;

    double operator*(const Vector2d& other) const;

    Vector2d operator*(const double scalar) const;

    double length();

    void normalize();

    void rotate(double angle);
};
#endif // VECTOR2D_H
