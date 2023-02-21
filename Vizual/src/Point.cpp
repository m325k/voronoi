#include "Point.h"

Point::Point(Vector2d position) {
    pos = position;
}

Point::~Point() {

}


double Point::getX() const {
    return pos.x;
}
double Point::getY() const {
    return pos.y;
}
