#pragma once

template<typename T>
inline const T& max(const T& a, const T& b) {
    return (a > b) ? a : b;
}

template<typename T>
inline const T& min(const T& a, const T& b) {
    return (a < b) ? a: b;
}

inline float deg2rad(const float &deg) {
    return deg * M_PI / 180;
}

inline float clamp(const float &lo, const float &hi, const float &v){
    return max(lo, min(hi, v));
}

template<typename T>
inline void swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

inline float modulo(const float &f) {
    return f - std::floor(f);
}

template<typename T>
T binpow (T a, int n) {
    T res = 1;
    while (n) {
        if (n & 1)
            res *= a;
        a *= a;
        n >>= 1;
    }
    return res;
}