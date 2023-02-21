#ifndef FIELD_H
#define FIELD_H
#include <vector>
#include <Point.h>
using namespace std;

class Field
{
    public:
        Field(int color, unsigned int size, Vector2d origin);
        virtual ~Field();
        vector< Point >& getPoints() {
            return points;
        }
        int getColor() {
            return m_color;
        }
    protected:

    private:
        vector < Point > points;
        int m_color;
};

#endif // FIELD_H
