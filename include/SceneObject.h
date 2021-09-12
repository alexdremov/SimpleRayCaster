#pragma once

#include "Matrix.h"
#include "Linalg.h"
#include "GeometryHelpers.h"
#include "Ray.h"
#include "Light.h"

class HittableObject {
public:
    virtual ~HittableObject() = default;

    [[nodiscard]] virtual bool intersect(const Ray& ray, float &tNear) const = 0;

    [[nodiscard]] virtual Vec3f getSurfaceNormal( const Vec3f &hitPoint, const Vec3f &viewDirection) const = 0;

    Vec3f albedo = 0.12;
    Vec3f ambient = 0.04;
    float Kd = 0.7;  // phong model diffuse weight
    float Ks = 0.9;  // phong model specular weight
    int n  = 10;     // phong specular exponent
    RGBColor color = {1, 1, 1};
};

class Sphere : public HittableObject {
public:
    Sphere(const Matrix4x4f &o2w, const float &r) : radius2(r * r) {
        center = o2w.multVecMatrix(Vec3f(0));
    }

    Sphere(const Vec3f &centerNew, const float &r) : center(centerNew), radius2(r * r) {}

    bool intersect(const Ray& ray, float &tNear) const override {
        const Vec3f& orig = ray.origin;
        const Vec3f& dir = ray.direction;
        float t0 = 0, t1 = 0;
        Vec3f L = orig - center;
        float a = dir.dotProduct(dir);
        float b = 2 * dir.dotProduct(L);
        float c = L.dotProduct(L) - radius2;
        if (!solveQuadratic(a, b, c, t0, t1))
            return false;

        if (t0 > t1)
            swap(t0, t1);

        if (t0 < 0) {
            t0 = t1;
            if (t0 < 0)
                return false;
        }
        tNear = t0 - kEpsilon * 10000;
        return true;
    }

    [[nodiscard]] Vec3f getSurfaceNormal(
            const Vec3f &hitPoint,
            const Vec3f &viewDirection) const override {
        Vec3f hitNormal = (hitPoint - center);
        hitNormal.normalize();
        return hitNormal;
    }

    float radius2;
    Vec3f center;
};

class MarkovaSphere : public HittableObject {
public:
    MarkovaSphere(const Matrix4x4f &o2w, const float &r) : radius2(r * r) {
        center = o2w.multVecMatrix(Vec3f(0));
    }

    MarkovaSphere(const Vec3f &centerNew, const float &r) : center(centerNew), radius2(r * r) {}

    bool intersect(const Ray& ray, float &tNear) const override {
        const Vec3f& orig = ray.origin;
        const Vec3f& dir = ray.direction;
        float t0 = 0, t1 = 0;
        Vec3f L = orig - center;
        float a = dir.dotProduct(dir);
        float b = 2 * dir.dotProduct(L);
        float c = L.dotProduct(L) - radius2;
        if (!solveQuadratic(a, b, c, t0, t1))
            return false;

        if (t0 > t1)
            swap(t0, t1);

        if (t0 < 0) {
            t0 = t1;
            if (t0 < 0)
                return false;
        }
        tNear = t0 - kEpsilon * 10000;
        return true;
    }

    [[nodiscard]] Vec3f getSurfaceNormal(
            const Vec3f &hitPoint,
            const Vec3f &viewDirection) const override {
        Vec3f hitNormal = (hitPoint - center);
        auto seedVec = hitPoint - center;
        float seed = (sin(seedVec[1] * 50) * cos(seedVec[0] * 50) * (sin(seedVec[2] * 50)));
        auto seedInt = unsigned((seedVec[1] * seedVec[0] * seedVec[2]) * 1000000.0f);
        srand(seedInt);
        hitNormal.normalize();
        hitNormal += Vec3f(rand(), rand(), rand()) * 0.00000000025;
        hitNormal *= (seed * seed + 0.7) / (1 + 0.7);
        return hitNormal;
    }

    float radius2;
    Vec3f center;
};

class Cube : public HittableObject {
public:
    Cube(const Matrix4x4f &o2w, const float &side): side(side) {
        Cube(o2w.multVecMatrix(Vec3f(0)), side);
    }

    Cube(const Vec3f &centerNew, const float &side) : center(centerNew), side(side) {
        minCorner = centerNew - side / 2;
        maxCorner = centerNew + side / 2;
    }

    bool intersect(const Ray& ray, float &tNear) const override {
        Vec3f T_1 = (minCorner - ray.origin) / ray.direction,
            T_2 = (maxCorner - ray.origin) / ray.direction;
        double t_near = -kInfinity;
        double t_far = kInfinity;

        for (int i = 0; i < 3; i++){
            if (ray.direction[i] == 0){
                if ((ray.origin[i] < minCorner[i]) || (ray.origin[i] > maxCorner[i])) {
                    return false;
                }
            } else {
                if(T_1[i] > T_2[i]){
                    swap(T_1,T_2);
                }
                if (T_1[i] > t_near){
                    t_near = T_1[i];
                }
                if (T_2[i] < t_far){
                    t_far = T_2[i];
                }
                if( (t_near > t_far) || (t_far < 0) ){
                    return false;
                }
            }
        }
        tNear = t_near - kEpsilon * 10000;
        return true;
    }

    [[nodiscard]] Vec3f getSurfaceNormal(
            const Vec3f &hitPoint,
            const Vec3f &viewDirection) const override {
        Vec3f normal = hitPoint - center;
        float maxcoord = max(fabs(normal[2]), max(fabs(normal[0]), fabs(normal[1])));

        normal *= Vec3f(fabs(normal[0]) == maxcoord,
                        fabs(normal[1]) == maxcoord,
                        fabs(normal[2]) == maxcoord);
        normal.normalize();
        return normal;
    }

    void setCenter(const Vec3f &newCenter) {
        center = newCenter;
        minCorner = center - side / 2;
        maxCorner = center + side / 2;
    }

    [[nodiscard]] const Vec3f &getCenter() const {
        return center;
    }

private:
    float side;
    Vec3f minCorner, maxCorner;
    Vec3f center;
};