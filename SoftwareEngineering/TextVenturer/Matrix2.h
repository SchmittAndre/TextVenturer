#pragma once
class Matrix2
{
private:
    float data[2][2];
public:
    Matrix2();
    ~Matrix2();

    void clear(float value = 0);

    float &operator()(int x, int y);

    float determinant();

};

