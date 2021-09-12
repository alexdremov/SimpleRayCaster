#pragma once
#include "Vector.h"

class Ray {


public:
    Vec3f direction;
    Vec3f origin;

    Ray(const Vec3f& origin, const Vec3f& direction) :origin(origin), direction(direction) {}
};