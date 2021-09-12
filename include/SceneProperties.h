#pragma once
#include "Matrix.h"

struct SceneOptions {
    uint32_t width = 640, height = 480;
    float fov = 55.7;
    RGBColor backgroundColor = Vec3f(0.01, 0.01, 0.01);
    uint32_t maxDepth = 5;

    Matrix4x4f cameraToWorld;
};