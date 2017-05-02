#include "stdafx.h"

#include "Color.h"         

const std::unordered_map<std::string, Color> Color::dictionary {
    // gray tones
    std::pair<std::string, Color>("white",         Color(1, 1, 1)),
    std::pair<std::string, Color>("light_gray",    Color(0.75f, 0.75f, 0.75f)),
    std::pair<std::string, Color>("gray",          Color(0.5f,  0.5f,  0.5f)),
    std::pair<std::string, Color>("dark_gray",     Color(0.25f, 0.25f, 0.25f)),
    std::pair<std::string, Color>("black",         Color(0, 0, 0)),      
    // rainbow colors                              
    std::pair<std::string, Color>("red",           Color(1, 0, 0)),
    std::pair<std::string, Color>("yellow",        Color(1, 1, 0)),
    std::pair<std::string, Color>("lime",          Color(0, 1, 0)),
    std::pair<std::string, Color>("cyan",          Color(0, 1, 1)),
    std::pair<std::string, Color>("blue",          Color(0, 0, 1)),
    std::pair<std::string, Color>("magenta",       Color(1, 0, 1)),
    // rainbow colors dark                         
    std::pair<std::string, Color>("dark_red",      Color(0.5, 0, 0)),
    std::pair<std::string, Color>("dark_yellow",   Color(0.5, 0.5, 0)),
    std::pair<std::string, Color>("dark_lime",     Color(0, 0.5, 0)),
    std::pair<std::string, Color>("dark_cyan",     Color(0, 0.5, 0.5)),
    std::pair<std::string, Color>("dark_blue",     Color(0, 0, 0.5)),
    std::pair<std::string, Color>("dark_magenta",  Color(0.5, 0, 0.5)),
    // rainbow colors light
    std::pair<std::string, Color>("light_red",     Color(1, 0.5, 0.5)),
    std::pair<std::string, Color>("light_yellow",  Color(1, 1, 0.5)),
    std::pair<std::string, Color>("light_lime",    Color(0.5, 1, 0.5)),
    std::pair<std::string, Color>("light_cyan",    Color(0.5, 1, 1)),
    std::pair<std::string, Color>("light_blue",    Color(0.5, 0.5, 1)),
    std::pair<std::string, Color>("light_magenta", Color(1, 0.5, 1))       
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

Color Color::operator*(float v) const
{
    return Color(r * v, g * v, b * v, a);
}

Color operator*(float v, const Color & color)
{
    return color * v;
}
