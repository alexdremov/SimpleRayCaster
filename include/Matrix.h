#pragma once
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include "Vector.h"

#if 0
template<typename T>
class Matrix4x4 {
public:
    T content[4][4] = {{1, 0, 0, 0},
                       {0, 1, 0, 0},
                       {0, 0, 1, 0},
                       {0, 0, 0, 1}};
    static const Matrix4x4 identity;

    inline const T& get(uint8_t row, uint8_t col) const {
        return content[row][col];
    }

    inline void set(uint8_t row, uint8_t col, const T& value) {
        content[row][col] = value;
    }

    Matrix4x4() = default;

    const T *operator[](uint8_t i) const {
        return content[i];
    }

    T *operator[](uint8_t i) {
        return content[i];
    }

    explicit Matrix4x4(const T data[4 * 4]) {
        memcpy(this->content, data, sizeof(T) * 16);
    }

    explicit Matrix4x4(const T data[4][4]) {
        memcpy(this->content, data, sizeof(T) * 16);
    }

    Matrix4x4 operator*(const Matrix4x4 &v) const {
        Matrix4x4 tmp;
        multiply(*this, v, tmp);
        return tmp;
    }

    static Matrix4x4 rotX(T rad){
        T data[4 * 4] = {
                1, 0, 0, 0,
                0, cos(rad), -sin(rad), 0,
                0,  sin(rad), cos(rad), 0,
                0, 0, 0, 1
        };
        return Matrix4x4(data);
    }

    static Matrix4x4 rotY(T rad){
        T data[4 * 4] = {
                cos(rad), 0, sin(rad), 0,
                0, 1, 0, 0,
                -sin(rad), 0, cos(rad), 0,
                0, 0, 0, 1
        };
        return Matrix4x4(data);
    }

    static Matrix4x4 rotZ(T rad){
        T data[4 * 4] = {
                cos(rad), -sin(rad), 0, 0,
                sin(rad), cos(rad), 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
        };
        return Matrix4x4(data);
    }

    static void multiply(const Matrix4x4<T> &a, const Matrix4x4 &b, Matrix4x4 &c) {
#pragma unroll
        for (uint8_t i = 0; i < 4; ++i) {
#pragma unroll
            for (uint8_t j = 0; j < 4; ++j) {
                c[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] +
                          a[i][2] * b[2][j] + a[i][3] * b[3][j];
            }
        }
    }

    Matrix4x4 transposed() const {
        Matrix4x4 t;
#pragma unroll
        for (uint8_t i = 0; i < 4; ++i) {
#pragma unroll
            for (uint8_t j = 0; j < 4; ++j) {
                t[i][j] = content[j][i];
            }
        }
        return t;
    }

    // \brief transpose itself
    Matrix4x4 &transpose() {
#pragma unroll
        for (uint8_t i = 0; i < 4; ++i) {
#pragma unroll
            for (uint8_t j = 0; j < 4; ++j) {
                T tmp = content[i][j];
                content[i][j] = content[j][i];
                content[j][i] = tmp;
            }
        }
        return *this;
    }

    template<typename S>
    [[nodiscard]] Vec3<S> multVecMatrix(const Vec3<S> &src) const {
        S w = src[0] * content[0][3] + src[1] * content[1][3] + src[2] * content[2][3] + content[3][3];
        S components[3] /* = {}*/;
#pragma unroll
        for(int i = 0; i < 3; i++)
            components[i] = (src[0] * content[0][i] + src[1] * content[1][i] +
                    src[2] * content[2][i] + content[3][i]) / w;

        return Vec3<S>(components[0], components[1], components[2]);
    }

    template<typename S>
    [[nodiscard]] Vec3<S> multDirMatrix(const Vec3<S> &src) const {
        S components[3] /* = {}*/;
#pragma unroll
        for(int i = 0; i < 3; i++)
            components[i] = (
                    src[0] * content[0][i] +
                    src[1] * content[1][i] +
                    src[2] * content[2][i]);

        return Vec3<S>(components[0], components[1], components[2]);
    }

    Matrix4x4 inverse() const {
        Matrix4x4 s = {};
        Matrix4x4 t(*this);

        for (int i = 0; i < 3; i++) {
            int pivot = i;

            T pivotsize = t[i][i];

            if (pivotsize < 0)
                pivotsize = -pivotsize;

            for (int j = i + 1; j < 4; j++) {
                T tmp = t[j][i];

                if (tmp < 0)
                    tmp = -tmp;

                if (tmp > pivotsize) {
                    pivot = j;
                    pivotsize = tmp;
                }
            }

            if (pivotsize == 0) {
                return Matrix4x4();
            }

            if (pivot != i) {
                for (int j = 0; j < 4; j++) {
                    T tmp;

                    tmp = t[i][j];
                    t[i][j] = t[pivot][j];
                    t[pivot][j] = tmp;

                    tmp = s[i][j];
                    s[i][j] = s[pivot][j];
                    s[pivot][j] = tmp;
                }
            }

            for (int j = i + 1; j < 4; j++) {
                T f = t[j][i] / t[i][i];

                for (int k = 0; k < 4; k++) {
                    t[j][k] -= f * t[i][k];
                    s[j][k] -= f * s[i][k];
                }
            }
        }

        for (int i = 3; i >= 0; --i) {
            T f = 0;

            if ((f = t[i][i]) == 0) {
                return Matrix4x4();
            }

            for (int j = 0; j < 4; j++) {
                t[i][j] /= f;
                s[i][j] /= f;
            }

            for (int j = 0; j < i; j++) {
                f = t[j][i];

                for (int k = 0; k < 4; k++) {
                    t[j][k] -= f * t[i][k];
                    s[j][k] -= f * s[i][k];
                }
            }
        }

        return s;
    }

    const Matrix4x4<T> &invert() {
        *this = inverse();
        return *this;
    }
};
#else
template<typename T>
class Matrix4x4 {
public:
    typedef T content16 __attribute__((matrix_type(4, 4)));
    content16 content;
    static const Matrix4x4 identity;

    Matrix4x4(){
        static const float identityData[] = {1, 0, 0, 0,
                                             0, 1, 0, 0,
                                             0, 0, 1, 0,
                                             0, 0, 0, 1};
        content = *((content16*)identityData);
    };

    inline const T& get(uint8_t row, uint8_t col) const {
        return content[row][col];
    }

    inline void set(uint8_t row, uint8_t col, const T& value) {
        content[row][col] = value;
    }

    explicit Matrix4x4(const T data[4 * 4]) {
        this->content = *((content16*)data);
    }

    explicit Matrix4x4(const T data[4][4]) {
        this->content = *((content16*)data);
    }

    Matrix4x4 operator*(const Matrix4x4 &v) const {
        Matrix4x4 tmp;
        multiply(*this, v, tmp);
        return tmp;
    }

    Matrix4x4& operator*=(const Matrix4x4 &v) {
        content *= v.content;
        return *this;
    }

    static Matrix4x4 rotX(T rad){
        T data[4 * 4] = {
                1, 0, 0, 0,
                0, cos(rad), -sin(rad), 0,
                0,  sin(rad), cos(rad), 0,
                0, 0, 0, 1
        };
        return Matrix4x4(data);
    }

    static Matrix4x4 rotY(T rad){
        T data[4 * 4] = {
                cos(rad), 0, sin(rad), 0,
                0, 1, 0, 0,
                -sin(rad), 0, cos(rad), 0,
                0, 0, 0, 1
        };
        return Matrix4x4(data);
    }

    static Matrix4x4 rotZ(T rad){
        T data[4 * 4] = {
                cos(rad), -sin(rad), 0, 0,
                sin(rad), cos(rad), 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
        };
        return Matrix4x4(data);
    }

    static void multiply(const Matrix4x4<T> &a, const Matrix4x4 &b, Matrix4x4 &c) {
        c.content = a.content * b.content;
    }

    Matrix4x4 transposed() const {
        Matrix4x4 t = *this;
        t.content = __builtin_matrix_transpose(t.content);
        return t;
    }

    Matrix4x4 &transpose() {
        content = __builtin_matrix_transpose(content);
        return *this;
    }

    template<typename S>
    [[nodiscard]] Vec3<S> multVecMatrix(const Vec3<S> &src) const {
        S w = src[0] * content[0][3] + src[1] * content[1][3] + src[2] * content[2][3] + content[3][3];
        S components[3] /* = {}*/;
#pragma unroll 3
        for(int i = 0; i < 3; i++)
            components[i] = (src[0] * content[0][i] + src[1] * content[1][i] +
                             src[2] * content[2][i] + content[3][i]) / w;

        return Vec3<S>(components[0], components[1], components[2]);
    }

    template<typename S>
    [[nodiscard]] Vec3<S> multDirMatrix(const Vec3<S> &src) const {
        S components[3] /* = {}*/;
#pragma unroll 3
        for(int i = 0; i < 3; i++)
            components[i] = (
                    src[0] * content[0][i] +
                    src[1] * content[1][i] +
                    src[2] * content[2][i]);

        return Vec3<S>(components[0], components[1], components[2]);
    }

    Matrix4x4 inverse() const {
        Matrix4x4 s = {};
        Matrix4x4 t(*this);

        for (int i = 0; i < 3; i++) {
            int pivot = i;

            T pivotsize = t[i][i];

            if (pivotsize < 0)
                pivotsize = -pivotsize;

            for (int j = i + 1; j < 4; j++) {
                T tmp = t[j][i];

                if (tmp < 0)
                    tmp = -tmp;

                if (tmp > pivotsize) {
                    pivot = j;
                    pivotsize = tmp;
                }
            }

            if (pivotsize == 0) {
                return Matrix4x4();
            }

            if (pivot != i) {
                for (int j = 0; j < 4; j++) {
                    T tmp;

                    tmp = t[i][j];
                    t[i][j] = t[pivot][j];
                    t[pivot][j] = tmp;

                    tmp = s[i][j];
                    s[i][j] = s[pivot][j];
                    s[pivot][j] = tmp;
                }
            }

            for (int j = i + 1; j < 4; j++) {
                T f = t[j][i] / t[i][i];

                for (int k = 0; k < 4; k++) {
                    t[j][k] -= f * t[i][k];
                    s[j][k] -= f * s[i][k];
                }
            }
        }

        for (int i = 3; i >= 0; --i) {
            T f = 0;

            if ((f = t[i][i]) == 0) {
                return Matrix4x4();
            }

            for (int j = 0; j < 4; j++) {
                t[i][j] /= f;
                s[i][j] /= f;
            }

            for (int j = 0; j < i; j++) {
                f = t[j][i];

                for (int k = 0; k < 4; k++) {
                    t[j][k] -= f * t[i][k];
                    s[j][k] -= f * s[i][k];
                }
            }
        }

        return s;
    }

    const Matrix4x4<T> &invert() {
        *this = inverse();
        return *this;
    }
};
#endif

typedef Matrix4x4<float> Matrix4x4f;
typedef Matrix4x4<double> Matrix44d;
typedef Matrix4x4<int> Matrix44i;