#pragma once

#include <cstdlib>
#include <cstdio>
#include <cmath>

template<typename T>
class Vec2 {
public:
    Vec2() : x(0), y(0) {}

    explicit Vec2(T xx) : x(xx), y(xx) {}

    Vec2(T xx, T yy) : x(xx), y(yy) {}

    Vec2 operator+(const Vec2 &v) const {
        return Vec2(x + v.x, y + v.y);
    }

    Vec2 operator/(const T &r) const {
        return Vec2(x / r, y / r);
    }

    Vec2 operator*(const T &r) const {
        return Vec2(x * r, y * r);
    }

    Vec2 &operator/=(const T &r) {
        x /= r, y /= r;
        return *this;
    }

    Vec2 &operator*=(const T &r) {
        x *= r, y *= r;
        return *this;
    }

    friend Vec2 operator*(const T &r, const Vec2<T> &v) { return Vec2(v.x * r, v.y * r); }

    T x, y;
};

#if 1

template<typename T>
class Vec3 {
    typedef T content3 __attribute__((ext_vector_type(3)));
public:
    Vec3(): content() {
        content = {0, 0, 0};
    }

    Vec3(T xx): content() {
        content = {xx, xx, xx};
    }

    explicit Vec3(content3 content): content() {
        this->content = content;
    }

    Vec3(T xx, T yy, T zz): content() {
        content = {xx, yy, zz};
    }

    Vec3 operator+(const Vec3 &v) const {
        return Vec3(content + v.content);
    }

    Vec3 operator-(const Vec3 &v) const {
        return Vec3(content - v.content);
    }

    Vec3 operator-() const {
        return Vec3(-content);
    }

    Vec3 operator*(const T &r) const {
        return Vec3(content * r);
    }

    Vec3 operator*(const Vec3 &v) const {
        return Vec3(content * v.content);
    }

    Vec3 operator/(const Vec3 &v) const {
        return Vec3(content / v.content);
    }

    [[nodiscard]] T dotProduct(const Vec3<T> &v) const {
        const auto res = content * v.content;
        return res[0] + res[1] + res[2];
    }

    Vec3 operator/(const T &r) const {
        return Vec3(content / r);
    }

    Vec3 &operator/=(const T &r) {
        content /= r;
        return *this;
    }

    Vec3 &operator*=(const T &r) {
        content *= r;
        return *this;
    }

    Vec3 &operator+=(const Vec3 &v) {
        content += v.content;
        return *this;
    }

    Vec3 &operator*=(const Vec3 &v) {
        content *= v.content;
        return *this;
    }

    Vec3 &operator=(T xx) {
        content = {xx, xx, xx};
        return *this;
    }

    [[nodiscard]] Vec3 crossProduct(const Vec3<T> &v) const {
        return Vec3<T>(content[1] * v.content[2] - content[2] * v.content[1],
                       content[2] * v.content[0] - content[0] * v.content[2],
                       content[0] * v.content[1] - content[1] * v.content[0]);
    }

    [[nodiscard]] T length2() const {
        const auto res = content * content;
        return res[0] + res[1] + res[2];
    }

    [[nodiscard]] T length() const {
        return sqrt(length2());
    }

    T operator[](uint8_t i) const {
        return content[i];
    }

    Vec3 &normalize() {
        const T n = length2();
        if (n > 0) {
            const T factor = 1 / sqrt(n);
            content *= factor;
        }
        return *this;
    }

    friend Vec3 operator*(const T &r, const Vec3 &v) {
        return Vec3<T>(v.content * r);
    }

    friend Vec3 operator/(const T &r, const Vec3 &v) {
        return Vec3<T>(r / v.content);
    }

    content3 content;
};
#else
template<typename T>
class Vec3 {
    typedef T content3[3];
public:
    Vec3() : content() {
        content[0] = content[1] = content[2] = 0;
    }

    Vec3(T xx){
        content[0] = content[1] = content[2] = xx;
    }

//    Vec3(content3 content) : content(content) {}

    Vec3(T xx, T yy, T zz) {
        content[0] = xx;
        content[1] = yy;
        content[2] = zz;
    }

    Vec3 operator+(const Vec3 &v) const {
        return Vec3(content[0] + v.content[0], content[1] + v.content[1], content[2] + v.content[2]);
    }

    Vec3 operator-(const Vec3 &v) const {
        return Vec3(content[0] - v.content[0], content[1] - v.content[1], content[2] - v.content[2]);
    }

    Vec3 operator-() const {
        return Vec3(-content[0], -content[1], -content[2]);
    }

    Vec3 operator*(const T &r) const {
        return Vec3(content[0] * r, content[1] * r, content[2] * r);
    }

    Vec3 operator*(const Vec3 &v) const {
        return Vec3(content[0] * v.content[0], content[1] * v.content[1], content[2] * v.content[2]);
    }

    T dotProduct(const Vec3<T> &v) const {
        return content[0] * v.content[0] + content[1] * v.content[1] + content[2] * v.content[2] ;
    }

    Vec3 operator/(const T &r) const {
        return Vec3(content[0] / r, content[1] / r, content[2] / r);
    }

    Vec3 &operator/=(const T &r) {
        content[0] /= r; content[1] /= r; content[2] /= r;
        return *this;
    }

    Vec3 &operator*=(const T &r) {
        content[0] *= r; content[1] *= r; content[2] *= r;
        return *this;
    }

    Vec3 &operator+=(const Vec3 &v) {
        content[0] += v.content[0]; content[1] += v.content[1]; content[2] += v.content[2];
        return *this;
    }

    Vec3& operator=(T xx){
        *this = Vec3(xx);
    }

    [[nodiscard]] Vec3 crossProduct(const Vec3<T> &v) const {
        return Vec3<T>(content[1] * v.content[2] - content[2] * v.content[1],
                       content[2] * v.content[0] - content[0] * v.content[2],
                       content[0] * v.content[1] - content[1] * v.content[0]);
    }

    [[nodiscard]] T length2() const {
        return content[0] * content[0] + content[1] * content[1] + content[2] * content[2];
    }

    [[nodiscard]] T length() const {
        return sqrt(length2());
    }

    const T &operator[](uint8_t i) const {
        return content[i];
    }

    Vec3 &normalize() {
        T n = length2();
        if (n > 0) {
            T factor = 1 / sqrt(n);
            content[0] *= factor;
            content[1] *= factor;
            content[2] *= factor;
        }

        return *this;
    }

    friend Vec3 operator*(const T &r, const Vec3 &v) {
        return Vec3<T>(v.content[0] * r, v.content[1] * r, v.content[2] * r);
    }

    friend Vec3 operator/(const T &r, const Vec3 &v) {
        return Vec3<T>(r / v.content[0], r / v.content[1], r / v.content[2]);
    }
    content3 content;
};
#endif

typedef Vec3<float> Vec3f;
typedef Vec3f RGBColor;
typedef Vec3<double> Vec3d;
typedef Vec3<int> Vec3i;

typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;
typedef Vec2<int> Vec2i;

inline Vec3f mix(const Vec3f &a, const Vec3f &b, const float &mixValue) {
    return a * (1 - mixValue) + b * mixValue;
}