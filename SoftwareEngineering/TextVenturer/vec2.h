#pragma once

template <typename type> 
struct gvec2 sealed
{
    gvec2();
    gvec2(type x, type y);
    
    gvec2<type> operator+(const gvec2<type> &other) const;
    gvec2<type> operator+(type value) const;
    template<typename t>
    friend gvec2<t> operator+(t value, const gvec2<t> &vec);

    gvec2<type> operator-() const;

    gvec2<type> operator-(const gvec2<type> &other) const;
    gvec2<type> operator-(type value) const;

    gvec2<type> operator*(const gvec2<type> &other) const;
    gvec2<type> operator*(type value) const;
    
    gvec2<type> operator/(const gvec2<type> &other) const;
    gvec2<type> operator/(type value) const;

    bool operator==(const gvec2<type> &other) const;
    bool operator!=(const gvec2<type> &other) const;

    gvec2<type> operator+=(const gvec2<type> &other);
    gvec2<type> operator-=(const gvec2<type> &other);

    gvec2<type> vectorTo(gvec2<type> &other) const;

    gvec2<type> cross() const;

    float dot(const gvec2<type> &other) const;

    float lengthSqr() const;
    float length() const;
    float getAngle() const;

    gvec2<type> rotate(float angle) const;

    gvec2<type> normalize() const;

    type x, y;
};
                      
typedef gvec2<int> ivec2;
typedef gvec2<size_t> uvec2;
                  
typedef gvec2<float> vec2;
typedef gvec2<float> texcoord;

template<typename type>
inline gvec2<type>::gvec2()
{
    x = 0;
    y = 0;
}

template<typename type>
inline gvec2<type>::gvec2(type x, type y)
{
    this->x = x;
    this->y = y;
}

template<typename type>
inline gvec2<type> gvec2<type>::operator+(const gvec2<type> &other) const
{
    return gvec2<type>(x + other.x, y + other.y);
}

template<typename type>
inline gvec2<type> gvec2<type>::operator+(type value) const
{
    return gvec2<type>(x + value, y + value);
}

template<typename t>
gvec2<t> operator+(t value, const gvec2<t> &vec)
{
    return gvec2<t>(value + vec.x, value + vec.y);
}

template<typename type>
inline gvec2<type> gvec2<type>::operator-() const
{
    return gvec2<type>(-x, -y);
}

template<typename type>
inline gvec2<type> gvec2<type>::operator-(const gvec2<type> &other) const
{
    return gvec2<type>(x - other.x, y - other.y);
}

template<typename type>
inline gvec2<type> gvec2<type>::operator-(type value) const
{
    return gvec2<type>(x - value, y - value);
}

template<typename type>
inline gvec2<type> gvec2<type>::operator*(const gvec2<type> &other) const
{
    return gvec2<type>(x * other.x, y * other.y);
}

template<typename type>
inline gvec2<type> gvec2<type>::operator*(type value) const
{
    return gvec2<type>(x * value, y * value);
}

template<typename type>
inline gvec2<type> gvec2<type>::operator/(const gvec2<type> &other) const
{
    return gvec2<type>(x / other.x, y / other.y);
}

template<typename type>
inline gvec2<type> gvec2<type>::operator/(type value) const
{
    return gvec2<type>(x / value, y / value);
}

template<typename type>
inline bool gvec2<type>::operator==(const gvec2<type>& other) const
{
    return x == other.x && y == other.y;
}

template<typename type>
inline bool gvec2<type>::operator!=(const gvec2<type>& other) const
{
    return !(*this == other);
}

template<typename type>
inline gvec2<type> gvec2<type>::operator+=(const gvec2<type>& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

template<typename type>
inline gvec2<type> gvec2<type>::operator-=(const gvec2<type>& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

template<typename type>
inline gvec2<type> gvec2<type>::vectorTo(gvec2<type> &other) const
{
    return other - *this;
}

template<typename type>
inline gvec2<type> gvec2<type>::cross() const
{
    return gvec2<type>(-y, x);
}

template<typename type>
inline float gvec2<type>::dot(const gvec2<type>& other) const
{
    return x * other.x + y * other.y;
}

template<typename type>
inline float gvec2<type>::lengthSqr() const
{
    return x * x + y * y;
}

template<typename type>
inline float gvec2<type>::length() const
{
    return sqrt(lengthSqr());
}

template<typename type>
inline float gvec2<type>::getAngle() const
{
    if (y > 0)
        return atan(x / y) * 180 / (float)PI;
    if (y < 0)
        return atan(x / y) * 180 / (float)PI + 180;
    else
    {
        if (x > 0)
            return 90;
        else if (x < 0)
            return 270;
        else
            return 0;
    }
}

template<typename type>
inline gvec2<type> gvec2<type>::rotate(float angle) const
{
    angle = angle * (type)PI / 180;
    return *this * cos(angle) + this->cross() * sin(angle);
}

template<typename type>
inline gvec2<type> gvec2<type>::normalize() const
{
    return *this / length();
}
