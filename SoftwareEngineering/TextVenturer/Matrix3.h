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

    float * ptr() const;

    float &operator()(int x, int y);
    float at(int x, int y) const;

    // matrix with matrix
    Matrix3 operator+(Matrix3 &other) const;
    Matrix3 operator-(Matrix3 &other) const;
    Matrix3 operator*(Matrix3 &other) const;
    
    // matrix with value
    Matrix3 operator*(float value) const;
    friend Matrix3 operator*(float value, const Matrix3 &mat);

    // std::vector with matrix
    friend vec2 operator*(const Matrix3 &mat, const vec2 &vec);

    Matrix3 transpose() const;
    float determinant() const;
    Matrix2 minor(int x, int y) const;
    float cofactor(int x, int y) const;
    Matrix3 cofactorMatrix() const;
    Matrix3 adjugate() const;
    Matrix3 inverse() const;
};

