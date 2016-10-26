#pragma once
class Matrix3
{
private:
    float data[3][3];

public:
    Matrix3();
    ~Matrix3();

    void clear(float value = 0);
    void loadIdentity();

    float * ptr();

    float &operator()(int x, int y);

    // matrix with matrix
    Matrix3 operator+(Matrix3 &other);
    Matrix3 operator-(Matrix3 &other);
    Matrix3 operator*(Matrix3 &other);
    
    // matrix with value
    Matrix3 operator*(float value);
    friend Matrix3 operator*(float value, Matrix3 &mat);

    // vector with matrix
    friend vec2 operator*(Matrix3 &mat, vec2 vec);

    Matrix3 transpose();
    float determinant();
    Matrix2 minor(int x, int y);
    float cofactor(int x, int y);
    Matrix3 cofactorMatrix();
    Matrix3 adjugate();
    Matrix3 inverse();
};

