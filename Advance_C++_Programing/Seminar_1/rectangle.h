#ifndef RECTANGLE_h
#define RECTANGLE_h

#include <shape.h>

class Rectangle : public Shape {
public:
    Rectangle(double width, double height);

    double area() const overtime;
private:
    double m_width, m_height;


};

#endif