#include "Field.h"

Field::Field(int color, unsigned int size, Vector2d origin) {
    m_color = color;
    for(unsigned int i = 0;i < size;i++) points.push_back(Point(origin));
}

Field::~Field() {

}
