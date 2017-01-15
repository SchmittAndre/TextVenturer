#include "stdafx.h"

#include "Color.h"         

const unordered_map<string, Color> Color::dictionary {
    // gray tones
    pair<string, Color>("white",        Color(1, 1, 1)),
    pair<string, Color>("light_gray",   Color(0.75f, 0.75f, 0.75f)),
    pair<string, Color>("gray",         Color(0.5f,  0.5f,  0.5f)),
    pair<string, Color>("dark_gray",    Color(0.25f, 0.25f, 0.25f)),
    pair<string, Color>("black",        Color(0, 0, 0)),      
    // rainbow colors
    pair<string, Color>("red",          Color(1, 0, 0)),
    pair<string, Color>("yellow",       Color(1, 1, 0)),
    pair<string, Color>("lime",         Color(0, 1, 0)),
    pair<string, Color>("cyan",         Color(0, 1, 1)),
    pair<string, Color>("blue",         Color(0, 0, 1)),
    pair<string, Color>("magenta",      Color(1, 0, 1))
    // other

};

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

Color Color::HSV(float h, float s, float v, float a)
{
    auto f = [](float h) -> float { return h * 6.0f - floor(h * 6.0f); };
    auto p = [](float v, float s) -> float { return v * (1 - s); };
    auto q = [](float v, float s, float f) -> float { return v * (1 - s * f); };
    auto t = [](float v, float s, float f) -> float { return v * (1 - s * (1 - f)); };

    if (h < 0)
        h -= floor(h / 6.0f) * 6.0f;
    h = fmod(h / 6.0f, 1.0f);
    switch ((int)floor(h * 6.0f))
    {
    case 0:
        return Color(v, t(v, s, f(h)), p(v, s));
    case 1:
        return Color(q(v, s, f(h)), v, p(v, s));
    case 2:
        return Color(p(v, s), v, t(v, s, f(h)));
    case 3:
        return Color(p(v, s), q(v, s, f(h)), v);
    case 4:
        return Color(t(v, s, f(h)), p(v, s), v);
    default:
        return Color(v, p(v, s), q(v, s, f(h)));
    }
}

void Color::toHSV(float & h, float & s, float & v)
{
    float maximum = max(r, max(g, b));
    float minimum = min(r, min(g, b));
    float diff = maximum - minimum;
    
    v = maximum;
    
    if (diff == 0 || maximum == 0)
        s = 0;
    else
        s = diff / maximum;
    
    if (s == 0)
        h = 0;
    else
    {
        float rc = (maximum - r) / diff;
        float gc = (maximum - g) / diff;
        float bc = (maximum - b) / diff;
        if (r == maximum)
            h = bc - gc;
        else if (g == maximum)
            h = 2 + rc - bc;
        else
            h = 4 + gc - rc;
        if (h < 0)
            h += 6;
    }
}

Color Color::rainbow(float hue, float a)
{
    return HSV(hue, 1, 1, a);
}

Color Color::addRainbow(float amount)
{
    float h, s, v;
    toHSV(h, s, v);
    return Color::HSV(h + amount, s, v);
}

bool Color::operator==(const Color other) const
{
    return r == other.r && g == other.g && b == other.b;
}
