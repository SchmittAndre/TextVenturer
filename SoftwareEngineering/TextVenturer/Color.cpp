#include "Color.h"



Color::Color(float r, float g, float b, float a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

Color::Color()
{
    r = 1;
    g = 1;
    b = 1;
    a = 1;
}

bool Color::operator==(const Color other)
{
    return r == other.r && g == other.g && b == other.b;
}
