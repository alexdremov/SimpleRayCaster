#include "Raycasting.h"
#include "FastList.h"

SceneOptions generateWorld(FastList<HittableObject *> &objects, FastList<Light *> &lights);

void freeWorld(const FastList<HittableObject *> &objects, const FastList<Light *> &lights);

void SDLInit(SDL_Window *&win, int *w, int *h);

int main() {
    FastList<HittableObject *> objects = {};
    FastList<Light *> lights = {};
    SceneOptions options = generateWorld(objects, lights);

    SDL_Window *win = nullptr;
    int w = 0, h = 0;
    const float scale = 1 / 1.0;
    SDLInit(win, &w, &h);
    options.width = w * scale;
    options.height = h * scale;

    SDL_Rect clipRect = {0, 0, w, h};

    SDL_Surface *content = createSurface(options.width, options.height);
    SDL_Surface *screen = SDL_GetWindowSurface(win);

    int close = 0;
    bool mousePressed = false;
    SDL_MouseMotionEvent pressPosition = {};

    float degY = 0.008;
    float degZ = 0.013;

    Matrix4x4f rotateViewMatrixY = Matrix4x4f::rotY(degY);
    Matrix4x4f rotateViewMatrixZ = Matrix4x4f::rotZ(degZ);

    Matrix4x4f rotateViewMatrix = rotateViewMatrixY * Matrix4x4f::rotX(0.01) * Matrix4x4f::rotZ(-0.02);
    Matrix4x4f rotateViewCompos = rotateViewMatrixY * rotateViewMatrixZ;

    Matrix4x4f rotateViewComposFirst = Matrix4x4f::rotX(-0.06) * Matrix4x4f::rotX(-0.002) * Matrix4x4f::rotZ(-0.01);
    Matrix4x4f rotateViewComposSecond = rotateViewCompos * Matrix4x4f::rotY(0.004);

    Matrix4x4f rotateViewComposSideFirst = Matrix4x4f::rotZ(-0.002) * Matrix4x4f::rotX(0.01) * Matrix4x4f::rotY(0.01);
    Matrix4x4f rotateViewComposSideSecond = Matrix4x4f::rotZ(0.002) * Matrix4x4f::rotX(-0.06) * Matrix4x4f::rotY(-0.007);

    Matrix4x4f rotateViewCubeFirst = Matrix4x4f::rotZ(0.009) * Matrix4x4f::rotX(-0.004) * Matrix4x4f::rotY(-0.01);
    Matrix4x4f rotateViewCubeSecond = Matrix4x4f::rotZ(-0.003) * Matrix4x4f::rotX(0.008) * Matrix4x4f::rotY(0.002);

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

    float wait = 0;

    while (!close) {
        auto timeStart = std::chrono::high_resolution_clock::now();

        options.cameraToWorld *= rotateViewMatrix;

        firstRotatable->center = rotateViewComposFirst.multVecMatrix(firstRotatable->center);
        secondRotatable->center = rotateViewComposSecond.multVecMatrix(secondRotatable->center);

        firstSideRotatable->center = rotateViewComposSideFirst.multVecMatrix(firstSideRotatable->center);
        secondSideRotatable->center = rotateViewComposSideSecond.multVecMatrix(secondSideRotatable->center);

        cubeFirst->setCenter(rotateViewCubeFirst.multVecMatrix(cubeFirst->getCenter()));
        cubeSecond->setCenter(rotateViewCubeSecond.multVecMatrix(cubeSecond->getCenter()));

        render(options, objects, lights, content);

        SDL_Event event = {};

        bool printed = false;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_MOUSEMOTION: {
                    if (mousePressed) {
                        SDL_MouseMotionEvent drag = event.motion;
                        pressPosition = event.motion;
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    mousePressed = true;
                    pressPosition = event.motion;
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    mousePressed = false;
                    break;
                }
                case SDL_MOUSEWHEEL: {
                    break;
                }
                case SDL_QUIT:
                    close = 1;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                            break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                            break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN: {
                            if(!printed)
                                IMG_SavePNG(content, "screensoot.png");
                            printed = true;
                            break;
                        }
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            break;
                        default:
                            break;
                    }
            }
        }
        SDL_BlitScaled(content, nullptr, screen, &clipRect);
        SDL_UpdateWindowSurface(win);

        auto timeEnd = std::chrono::high_resolution_clock::now();
        auto passedTime = std::chrono::duration<float, std::milli>(timeEnd - timeStart).count();
        fprintf(stderr, "\rReal FPS: %.2f ", 1.0f / passedTime * 1000);
        const float fpsTarget = 35;
        if (passedTime / 1000 < 1 / fpsTarget) {
            wait = 1000.0f / fpsTarget - passedTime;
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

void SDLInit(SDL_Window *&win, int *w, int *h) {
    win = SDL_CreateWindow("Graph", // creates a window
                           SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED,
                           1920, 1080,
                           SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 2);
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_GL_GetDrawableSize(win, w, h);
}


SceneOptions generateWorld(FastList<HittableObject *> &objects, FastList<Light *> &lights) {
    SceneOptions options = {};

    options.cameraToWorld.set(3, 2, 15);
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

    cubeOne->n = cubeTwo->n = 15;
    cubeOne->color = {108.0f / 255, 216.0f / 255, 212.0f / 255};
    cubeTwo->color = {216.0f / 255, 108.0f / 255, 112.0f / 255};

    objects.pushBack(cubeOne);
    objects.pushBack(cubeTwo);

    lights.pushBack(new PointLight({10, 10, 10},
                                   {216.0f / 255, 108.0f / 255, 112.0f / 255}, 19000));
    lights.pushBack(new PointLight({-10, 10, -10},
                                   {201.0f / 255, 160.0f / 255, 220.0f / 255}, 19000));
    lights.pushBack(new DistantLight({0, -10, 0},
                                     {118.0f / 255, 196.0f / 255, 174.0f / 255}, 3));
    return options;
}
