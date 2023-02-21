#ifndef POINT_H
#define POINT_H
#include <Vector2d.h>

class Point {
public:
    Point(Vector2d position);
    virtual ~Point();
    double getX() const;
    double getY() const;
    Vector2d pos;
    Vector2d vel;
    Vector2d acc;
};

#endif // POINT_H
