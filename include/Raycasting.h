#pragma once

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <utility>
#include <cstdint>
#include <cmath>
#include <chrono>
#include <thread>

#include "FastList.h"
#include "Matrix.h"
#include "Vector.h"
#include "GeometryHelpers.h"
#include "SceneObject.h"
#include "SceneProperties.h"
#include "Light.h"
#include "SDLHelpers.h"

HittableObject *trace(const Vec3f &orig, const Vec3f &dir, const FastList<HittableObject *> &objects, float &tNear) {
    float nearest = kInfinity;
    HittableObject * nearestObj = nullptr;
    for (size_t objectIndex = objects.begin(); objectIndex != objects.end(); objects.nextIterator(&objectIndex)) {
        HittableObject *object = nullptr;
        objects.get(objectIndex, &object);
        if (object->intersect({orig, dir}, tNear)) {
            if (tNear < nearest) {
                nearestObj = object;
                nearest = tNear;
            }
        }
    }
    tNear = nearest;
    return nearestObj;
}

inline Vec3f reflect(const Vec3f &I, const Vec3f &N) {
    return I - 2 * I.dotProduct(N) * N;
}

RGBColor castRay(
        const Vec3f &orig, const Vec3f &dir,
        const FastList<HittableObject *> &objects,
        const FastList<Light *> &lights,
        const SceneOptions &options,
        const uint32_t &depth = 0) {
    if (depth > options.maxDepth)
        return options.backgroundColor;

    HittableObject *object = nullptr;
    float tNear = 0;
    RGBColor hitColor = {};
    if ((object = trace(orig, dir, objects, tNear))) {
        Vec3f hitPoint = orig + dir * tNear;
        Vec3f hitNormal = object->getSurfaceNormal(hitPoint, dir);
        Vec3f diffuse = 0, specular = 0;
        for (size_t lightIndex = lights.begin(); lightIndex != lights.end(); lights.nextIterator(&lightIndex)) {
            Light *light = nullptr;
            lights.get(lightIndex, &light);
            RGBColor lightDir = {}, lightIntensity = {};
            light->illuminate(hitPoint, lightDir, lightIntensity, tNear);

            bool vis = !trace(hitPoint + hitNormal, -lightDir, objects, tNear);

            diffuse += vis * lightIntensity *
                       max(0.f, hitNormal.dotProduct(-lightDir));

            RGBColor R = reflect(lightDir, hitNormal);
            specular += vis * lightIntensity * binpow(max(0.f, R.dotProduct(-dir)), (int) object->n);
        }
        hitColor = object->albedo * diffuse * object->Kd * object->color + specular * object->Ks * object->color + object->ambient;
    } else {
        hitColor = options.backgroundColor;
    }

    return hitColor;
}

void threadedRend(const SceneOptions &options, const FastList<HittableObject *> &objects,
                  const FastList<Light *> &lights, SDL_Surface *surface, int id=0, int threadsCount=8) {
    const float scale = tan(deg2rad(options.fov * 0.5));
    const float imageAspectRatio = options.width / (float) options.height;
    const Vec3f orig = options.cameraToWorld.multVecMatrix(Vec3f(0));
    for (int j = id; j < options.height; j+=threadsCount) {
        for (int i = 0; i < options.width; ++i) {
            float x = (2 * (i + 0.5f) / (float) options.width - 1) * imageAspectRatio * scale;
            float y = (1 - 2 * (j + 0.5f) / (float) options.height) * scale;
            Vec3f dir = options.cameraToWorld.multDirMatrix(Vec3f(x, y, -1));
            dir.normalize();
            RGBColor color = castRay(orig, dir, objects, lights, options);
            const float deltaAdjustment = 0.55;
            color = RGBColor(pow(color[0], deltaAdjustment), pow(color[1], deltaAdjustment), pow(color[2], deltaAdjustment) ) * 255;
            setPixel(surface, i, j, ColorToUint(
                    clamp(0, 255, color[0]),
                    clamp(0, 255, color[1]),
                    clamp(0, 255, color[2]),
                    255)
            );
        }
        if (id == 0) {
            fprintf(stderr, "\rCompletion: %.2f", 100.0f * j / options.height);
        }
    }
}

void render(const SceneOptions &options, const FastList<HittableObject *> &objects,
            const FastList<Light *> &lights, SDL_Surface *surface, int threadsCount=8) {
    using std::thread;
    auto** threads = static_cast<thread**>(calloc(threadsCount, sizeof(thread*)));

    for (int i=0; i < threadsCount; i++) {
        threads[i] = new thread(threadedRend, options, objects, lights, surface, i, threadsCount);
    }
    for (int i=0; i < threadsCount; i++) {
        threads[i]->join();
        delete threads[i];
    }
    free(threads);
}