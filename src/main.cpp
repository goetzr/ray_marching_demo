#include "pch.h"

#include <cstdint>
#include <cmath>
#include <iostream>
#include <memory>

#include <SDL3/SDL.h>

// Aspect ratio = ~1.6:1.
constexpr int WIDTH  = 1024;
constexpr int HEIGHT = 640;

// ----------------------------------------------------------------------
// Your raymarcher will eventually compute a color per pixel.
// For now, this is just a placeholder.
// ----------------------------------------------------------------------
uint32_t raymarch_pixel(int x, int y)
{
    // Example: simple gradient
    uint8_t r = (uint8_t)((float)x / WIDTH * 255);
    uint8_t g = (uint8_t)((float)y / HEIGHT * 255);
    uint8_t b = 128;

    return (0xFF << 24) | (r << 16) | (g << 8) | b;  // ARGB8888
}

int main(int argc, char** argv)
{
    // ----------------------------------------------------------------------
    // Initialize SDL and create the window
    // ----------------------------------------------------------------------
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Raymarching Demo",
        WIDTH, HEIGHT,
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        return 1;
    }

    // ----------------------------------------------------------------------
    // Create renderer and streaming texture.
    // A streaming texture is the fastest CPU blit (block transfer) path.
    // ----------------------------------------------------------------------
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH, HEIGHT
    );

    if (!texture) {
        std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << "\n";
        return 1;
    }

    // Allocate the CPU pixel buffer
    std::unique_ptr<uint32_t[]> pixels = std::make_unique<uint32_t[]>(WIDTH * HEIGHT);

    // ----------------------------------------------------------------------
    // Main Loop
    // ----------------------------------------------------------------------
    SDL_Event event;
    bool done = false;
    while (true) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }
        if (done) {
            break;
        }

        // Compute pixels on CPU
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                pixels[y * WIDTH + x] = 0xffff0000;
                // pixels[y * WIDTH + x] = raymarch_pixel(x, y);
            }
        }

        // Update SDL texture with pixel buffer
        SDL_UpdateTexture(texture, nullptr, pixels.get(), WIDTH * sizeof(uint32_t));

        // Render the texture to the window
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    // ----------------------------------------------------------------------
    // Cleanup
    // ----------------------------------------------------------------------
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}