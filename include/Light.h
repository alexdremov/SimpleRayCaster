#pragma once

static const float kInfinity = std::numeric_limits<float>::max();
static const float kEpsilon = 1e-8;

class Light {
public:
    explicit Light(const Vec3f &c = 1, const float &i = 1) : color(c), intensity(i) {}

    virtual ~Light() = default;

    virtual void illuminate(const Vec3f &P, Vec3f &, Vec3f &, float &) const = 0;

    RGBColor color;
    float intensity;
};

class DistantLight : public Light {
    Vec3f dir;
public:
    explicit DistantLight(const Matrix4x4f &l2w, const RGBColor &c = 1, const float &i = 1) : Light(c, i) {
        dir = l2w.multDirMatrix(Vec3f(0, 0, -1));
        dir.normalize();
    }

    explicit DistantLight(const Vec3f& dir, const RGBColor &c = 1, const float &i = 1) : Light(c, i), dir(dir) {
        this->dir.normalize();
    }

    void illuminate(const Vec3f &P, Vec3f &lightDir, Vec3f &lightIntensity, float &distance) const override {
        lightDir = dir;
        lightIntensity = color * intensity;
        distance = kInfinity;
    }
};


class PointLight : public Light {
    Vec3f pos;
public:
    explicit PointLight(const Matrix4x4f &l2w, const RGBColor &c = 1, const float &i = 1) : Light(c, i) {
        pos = l2w.multDirMatrix(Vec3f(0));
    }

    explicit PointLight(const Vec3f& pos, const RGBColor &c = 1, const float &i = 1) : Light(c, i), pos(pos) {
    }

    void illuminate(const Vec3f &P, Vec3f &lightDir, Vec3f &lightIntensity, float &distance) const override {
        lightDir = (P - pos);
//        printf("%f %f %f\n", pos[0], pos[1], pos[2]);
        float r2 = lightDir.length2();
        distance = sqrt(r2);
        lightDir /= distance;
        lightIntensity = color * intensity / (4 * M_PI * r2);
//        printf("%f %f %f\n", lightIntensity[0], lightIntensity[1], lightIntensity[2]);
    }
};