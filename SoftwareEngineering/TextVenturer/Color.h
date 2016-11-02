#pragma once
struct Color
{
    float r, g, b, a;

    Color(float r, float g, float b, float a = 1);
    Color();

    bool operator==(const Color other);
};

