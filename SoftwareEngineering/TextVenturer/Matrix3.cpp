#include "stdafx.h"

Matrix3::Matrix3()
{
    clear();
}                  

Matrix3::~Matrix3()
{
}

void Matrix3::clear(float value)
{
    if (value == 0)
        memset(data, 0, sizeof(data));
    else
        for (int x = 0; x < 3; x++)
            for (int y = 0; y < 3; y++)
                data[x][y] = value;
}

void Matrix3::loadIdentity()
{
    clear();
    data[0][0] = 1;
    data[1][1] = 1;
    data[2][2] = 1;
}

float * Matrix3::ptr() const
{
    return (float*)data;
}

float &Matrix3::operator()(int x, int y)
{
    return data[x][y];
}

float Matrix3::at(int x, int y) const
{
    return data[x][y];
}

Matrix3 Matrix3::operator+(Matrix3 & other) const
{
    Matrix3 result;
    for (int x = 0; x < 3; x++)
        for (int y = 0; y < 3; y++)
            result(x, y) = at(x, y) + other(x, y);
    return result;
}

Matrix3 Matrix3::operator-(Matrix3 & other) const
{
    Matrix3 result;
    for (int x = 0; x < 3; x++)
        for (int y = 0; y < 3; y++)
            result(x, y) = at(x, y) - other(x, y);
    return result;
}

Matrix3 Matrix3::operator*(Matrix3 & other) const
{
    Matrix3 result;
    for (int x = 0; x < 3; x++)
        for (int y = 0; y < 3; y++)
            result(x, y) = at(0, y) * other(x, 0) + at(1, y) * other(x, 1) + at(2, y) * other(x, 2);
    return result;
}

Matrix3 Matrix3::operator*(float value) const
{
    Matrix3 result;
    for (int x = 0; x < 3; x++)
        for (int y = 0; y < 3; y++)
            result(x, y) = at(x, y) * value;
    return result;
}

Matrix3 Matrix3::transpose() const
{
    Matrix3 result;
    for (int x = 0; x < 3; x++)
        for (int y = 0; y < 3; y++)
            result(x, y) = at(x, y);
    return result;
}

float Matrix3::determinant() const
{
    return data[0][0] * data[1][1] * data[2][2] +
           data[0][1] * data[1][2] * data[2][0] +
           data[0][2] * data[1][0] * data[2][1] -
           data[2][0] * data[1][1] * data[0][2] -
           data[2][1] * data[1][2] * data[0][0] -
           data[2][2] * data[1][0] * data[0][1];
}

Matrix2 Matrix3::minor(int x, int y) const
{
    Matrix2 result;
    int x1, y1, x2, y2;
    x2 = 0;
    for (x1 = 0; x1 < 3; x1++)
    {
        if (x1 == x)
            continue;
        y2 = 0;
        for (y1 = 0; y1 < 3; y1++)
        {
            if (y1 == y)
                continue;
            result(x2, y2) = data[x1][y1];
            y2++;
        }
        x2++;
    }
    return result;
}

float Matrix3::cofactor(int x, int y) const
{
    if (((x + y) % 2) == 0)
        return minor(x, y).determinant();
    else
        return -minor(x, y).determinant();
}

Matrix3 Matrix3::cofactorMatrix() const
{
    Matrix3 result;
    for (int x = 0; x < 3; x++)
        for (int y = 0; y < 3; y++)
            result(x, y) = cofactor(x, y);
    return result;
}

Matrix3 Matrix3::adjugate() const
{
    return cofactorMatrix().transpose();
}

Matrix3 Matrix3::inverse() const
{
    return 1 / determinant() * adjugate();
}

Matrix3 operator*(float value, const Matrix3 & mat)
{
    return mat * value;
}

vec2 operator*(const Matrix3 & mat, const vec2 & vec)
{
    vec2 result;
    result.x = mat.at(0, 0) * vec.x + mat.at(0, 1) * vec.y + mat.at(0, 2);
    result.y = mat.at(1, 0) * vec.x + mat.at(1, 1) * vec.y + mat.at(1, 2);
    return result;
}
