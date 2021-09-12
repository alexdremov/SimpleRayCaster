#pragma once
#include <SDL.h>
#include <SDL_image.h>

void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
    auto *const target_pixel = (Uint32 *) ((Uint8 *) surface->pixels
                                           + y * surface->pitch
                                           + x * surface->format->BytesPerPixel);
    *target_pixel = pixel;
}

inline Uint32 getPixel(SDL_Surface *surface, int x, int y) {
    return *(Uint32 *) ((Uint8 *) surface->pixels
                        + y * surface->pitch
                        + x * surface->format->BytesPerPixel);
}

inline Uint32* getPixelPtr(SDL_Surface *surface, int x, int y) {
    return (Uint32 *) ((Uint8 *) surface->pixels
                       + y * surface->pitch
                       + x * surface->format->BytesPerPixel);
}

void WipeSurface(SDL_Surface *surface) {
    SDL_LockSurface(surface);
    SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 0, 0, 0));
    SDL_UnlockSurface(surface);
}

SDL_Surface *createSurface(int width, int height) {
    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    auto surface = SDL_CreateRGBSurface(0, width, height, 32,
                                        rmask,
                                        gmask,
                                        bmask,
                                        amask);
    if (surface == nullptr) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
    }
    return surface;
}

Uint32 ColorToUint(int r, int g, int b, int a)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    return (Uint32)((r << 24) + (g << 16) + (b << 8) + (a << 0));
#else
    return (Uint32)((a << 24) + (b << 16) + (g << 8) + (r << 0));
#endif
}

inline Uint32 ColorToUint(SDL_Color color){
    return ColorToUint(color.r, color.g, color.b, color.a);
}

inline SDL_Color UintToColor(Uint32 color)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
static const Uint32 rmask = 0xff000000;
static const Uint32 gmask = 0x00ff0000;
static const Uint32 bmask = 0x0000ff00;
static const Uint32 amask = 0x000000ff;
#else
    static const Uint32 rmask = 0x000000ff;
    static const Uint32 gmask = 0x0000ff00;
    static const Uint32 bmask = 0x00ff0000;
    static const Uint32 amask = 0xff000000;
#endif
    SDL_Color tempcol = {};
    tempcol.a = color & amask;
    tempcol.r = color & rmask;
    tempcol.g = color & gmask;
    tempcol.b = color & bmask;
    return tempcol;
}

inline void freeSurface(SDL_Surface *surface) {
    SDL_FreeSurface(surface);
}

inline void saveSurface(SDL_Surface *surface, const char *file) {
    IMG_SavePNG(surface, file);
}