#include "Raycasting.h"
#include "FastList.h"

SceneOptions generateWorld(FastList<HittableObject *> &objects, FastList<Light *> &lights);

void freeWorld(const FastList<HittableObject *> &objects, const FastList<Light *> &lights);

void SDLInit(SDL_Window *&win, int *w, int *h);

void eventLoop(SDL_Surface *content, SDL_Event &event, bool printed, const float moveStep, SceneOptions &options,
               int &close);


inline Matrix4x4f getRandRot(float modify = 100) {
    return Matrix4x4f::rot((rand() / (float)RAND_MAX - 0.5) / modify,
                           (rand() / (float)RAND_MAX - 0.5) / modify,
                           (rand() / (float)RAND_MAX - 0.5) / modify);
}

int main() {
    FastList<HittableObject *> objects = {};
    FastList<Light *> lights = {};
    SceneOptions options = generateWorld(objects, lights);

    SDL_Window *win = nullptr;
    int w = 0, h = 0;
    const float scale = 1 / 2.0;
    SDLInit(win, &w, &h);
    options.width = w * scale;
    options.height = h * scale;

    SDL_Rect clipRect = {0, 0, w, h};

    SDL_Surface *content = createSurface(options.width, options.height);
    SDL_Surface *screen = SDL_GetWindowSurface(win);

    Matrix4x4f rotateViewMatrix = getRandRot(100),rotateViewCompos = getRandRot(100);
    Matrix4x4f rotateViewComposFirst = getRandRot(10), rotateViewComposSecond = getRandRot(10);
    Matrix4x4f rotateViewComposSideFirst = getRandRot(10), rotateViewComposSideSecond = getRandRot(10);
    Matrix4x4f rotateViewCubeFirst = getRandRot(10), rotateViewCubeSecond = getRandRot(10);

    HittableObject *firstRotatableObj = nullptr, *secondRotatableObj = nullptr;
    HittableObject *sideFirstRotatableObj = nullptr, *sideSecondRotatableObj = nullptr;
    HittableObject *cubeFirstObj = nullptr, *cubeSecondObj = nullptr;

    objects.get(2, &firstRotatableObj);
    objects.get(4, &secondRotatableObj);

    objects.get(1, &sideFirstRotatableObj);
    objects.get(5, &sideSecondRotatableObj);

    objects.get(6, &cubeFirstObj);
    objects.get(7, &cubeSecondObj);

    auto *firstRotatable = dynamic_cast<MarkovaSphere *>(firstRotatableObj),
            *secondRotatable = dynamic_cast<MarkovaSphere *>(secondRotatableObj),
            *firstSideRotatable = dynamic_cast<MarkovaSphere *>(sideFirstRotatableObj),
            *secondSideRotatable = dynamic_cast<MarkovaSphere *>(sideSecondRotatableObj);

    auto *cubeFirst = dynamic_cast<Cube *>(cubeFirstObj),
            *cubeSecond = dynamic_cast<Cube *>(cubeSecondObj);

    int close = 0;
    while (!close) {
        auto timeStart = std::chrono::high_resolution_clock::now();

        firstRotatable->center = rotateViewComposFirst.multVecMatrix(firstRotatable->center);
        secondRotatable->center = rotateViewComposSecond.multVecMatrix(secondRotatable->center);

        firstSideRotatable->center = rotateViewComposSideFirst.multVecMatrix(firstSideRotatable->center);
        secondSideRotatable->center = rotateViewComposSideSecond.multVecMatrix(secondSideRotatable->center);

        cubeFirst->setCenter(rotateViewCubeFirst.multVecMatrix(cubeFirst->getCenter()));
        cubeSecond->setCenter(rotateViewCubeSecond.multVecMatrix(cubeSecond->getCenter()));

        render(options, objects, lights, content);

        SDL_Event event = {};
        bool printed = false;
        const float moveStep = 0.1;

        eventLoop(content, event, printed, moveStep, options, close);

        SDL_BlitScaled(content, nullptr, screen, &clipRect);
        SDL_UpdateWindowSurface(win);

        auto timeEnd = std::chrono::high_resolution_clock::now();
        auto passedTime = std::chrono::duration<float, std::milli>(timeEnd - timeStart).count();
        fprintf(stderr, "\rReal FPS: %.2f ", 1.0f / passedTime * 1000);
        const float fpsTarget = 35;
        if (passedTime / 1000 < 1 / fpsTarget) {
            float wait = 1000.0f / fpsTarget - passedTime;
            SDL_Delay(wait);
        }
        timeEnd = std::chrono::high_resolution_clock::now();
        passedTime = std::chrono::duration<float, std::milli>(timeEnd - timeStart).count();
        float fps = 1.0f / (passedTime / 1000);
        fprintf(stderr, "FPS: %.2f\n", fps);
    }

    freeWorld(objects, lights);
    return 0;
}

void eventLoop(SDL_Surface *content, SDL_Event &event, bool printed, const float moveStep, SceneOptions &options,
               int &close) {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                close = 1;
                break;
            case SDL_KEYDOWN:
                Vec3f dirForward = options.cameraToWorld.multDirMatrix(Vec3f(0, 0, -1)) * moveStep;
                Vec3f dirDown = options.cameraToWorld.multDirMatrix(Vec3f(0, 1, 0)) * moveStep;
                Vec3f dirLeft = options.cameraToWorld.multDirMatrix(Vec3f(-1, 0, 0)) * moveStep;
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:{
                        options.cameraToWorld *= Matrix4x4f::translate(dirForward);
                        break;
                    }
                    case SDL_SCANCODE_S:{
                        options.cameraToWorld *= Matrix4x4f::translate(-dirForward);
                        break;
                    }
                    case SDL_SCANCODE_A:{
                        options.cameraToWorld *= Matrix4x4f::translate(dirLeft);
                        break;
                    }
                    case SDL_SCANCODE_D:{
                        options.cameraToWorld *= Matrix4x4f::translate(-dirLeft);
                        break;
                    }
                    case SDL_SCANCODE_Q:{
                        options.cameraToWorld *= Matrix4x4f::rotY(-moveStep);
                        break;
                    }
                    case SDL_SCANCODE_E:{
                        options.cameraToWorld *= Matrix4x4f::rotY(moveStep);
                        break;
                    }
                    case SDL_SCANCODE_Z:{
                        options.cameraToWorld *= Matrix4x4f::translate(dirDown);
                        break;
                    }
                    case SDL_SCANCODE_X:{
                        options.cameraToWorld *= Matrix4x4f::translate(-dirDown);
                        break;
                    }
                    case SDL_SCANCODE_F:{
                        if(!printed)
                            IMG_SavePNG(content, "screensoot.png");
                        printed = true;
                        break;
                    }
                    case SDL_SCANCODE_UP:{
                        options.cameraToWorld *= Matrix4x4f::rotX(moveStep);
                        break;
                    }
                    case SDL_SCANCODE_DOWN:{
                        options.cameraToWorld *= Matrix4x4f::rotX(-moveStep);
                        break;
                    }
                    default:
                        break;
                }
        }
    }
}

void freeWorld(const FastList<HittableObject *> &objects, const FastList<Light *> &lights) {
    for (size_t i = objects.begin(); i != objects.end(); objects.nextIterator(&i)) {
        HittableObject *object = nullptr;
        objects.get(i, &object);
        delete object;
    }

    for (size_t i = lights.begin(); i != lights.end(); lights.nextIterator(&i)) {
        Light *light = nullptr;
        lights.get(i, &light);
        delete light;
    }
}


SceneOptions generateWorld(FastList<HittableObject *> &objects, FastList<Light *> &lights) {
    SceneOptions options = {};

    options.cameraToWorld.set(3, 2, 10);
    options.cameraToWorld.set(3, 1, 1);
    options.cameraToWorld.set(3, 0, 0);

    float w[5] = {0.01, 0.06, 0.1, 0.15, 0.2};
    float r[5] = {0.7, 0.7, 2, 0.7, 0.7};
    Vec3f colors[5] = {{255.0f / 255, 242.0f / 255, 204.0f / 255},
                       {237.0f / 255, 85.0f / 255,  59.0f / 255},
                       {207.0f / 255, 227.0f / 255, 226.0f / 255},
                       {32.0f / 255,  99.0f / 255,  155.0f / 255},
                       {253.0f / 255, 50.0f / 255, 89.0f / 255}};
    for (int i = -4, n = 2, k = 0; i <= 4; i += 2, n *= 3, k++) {
        Matrix4x4f xformSphere = {};
        xformSphere.set(3, 0, i * 1.8);
        xformSphere.set(3, 1, 0);
        HittableObject *sph = nullptr;
        if (i != 0)
            sph = new MarkovaSphere(xformSphere, r[k]);
        else
            sph = new Sphere({0, 0, 0}, r[k]);
        sph->n = n;
        sph->Ks = w[k];
        sph->color = colors[k];
        objects.pushBack(sph);
    }
    HittableObject *cubeOne = new Cube({2.5, -2.5, 2.5}, 1),
            *cubeTwo = new Cube({-2.5, 2.5, -2.5}, 1);

    cubeOne->n = cubeTwo->n = 16;
    cubeOne->color = {108.0f / 255, 216.0f / 255, 212.0f / 255};
    cubeTwo->color = {216.0f / 255, 108.0f / 255, 112.0f / 255};

    objects.pushBack(cubeOne);
    objects.pushBack(cubeTwo);

    objects.pushBack(new MarkovaSphere({0, 0, 0}, 0.8));

    lights.pushBack(new PointLight({10, 10, 10},
                                   {216.0f / 255, 108.0f / 255, 112.0f / 255}, 19000));
    lights.pushBack(new PointLight({-10, 10, -10},
                                   {201.0f / 255, 160.0f / 255, 220.0f / 255}, 19000));
    lights.pushBack(new DistantLight({0, -10, 0},
                                     {118.0f / 255, 196.0f / 255, 174.0f / 255}, 3));
    return options;
}