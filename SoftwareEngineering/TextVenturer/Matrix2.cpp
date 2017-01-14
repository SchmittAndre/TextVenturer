#include "stdafx.h"

Matrix2::Matrix2()
{
    clear();
}

Matrix2::~Matrix2()
{
}

void Matrix2::clear(float value)
{
    if (value == 0)
        memset(data, 0, sizeof(data));
    else
        for (int x = 0; x < 2; x++)
            for (int y = 0; y < 2; y++)
                data[x][y] = value;
}

float & Matrix2::operator()(int x, int y)
{
    return data[x][y];
}

float Matrix2::at(int x, int y) const
{
    return data[x][y];
}

float Matrix2::determinant() const
{
    return data[0][0] * data[1][1] - data[0][1] * data[1][0];
}
