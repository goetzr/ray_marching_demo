#include "pch.h"

#include <SDL3/SDL.h>

#include "vec3.h"
#include "camera.h"
#include "scene.h"

constexpr int WIDTH  = 800;
constexpr int HEIGHT = 600;

/**
 * Raymarch the specified pixel to determine its color.
 * 
 * @param scene   The scene holding the elements to render.
 * @param camera  The camera.
 * @param x       The pixel's x coordinate.
 * @param y       The pixel's y coordinate.
 * 
 * @return Returns the pixel's color in ARGB8888 format.
 */
uint32_t raymarch_pixel(const Scene& scene, const Camera& camera, int x, int y)
{
    // A ray that has been marched this many steps is defined to have missed all elements in the scene.
    static constexpr int MaxSteps = 10;
    // A ray has hit an element in the scene if its distance to that element is less than Epsilon.
    static constexpr double Epsilon = 0.001;

    // Generate the ray direction.
    Vec3 ray_dir = (Vec3(x, y, 0) - camera.position).normalize();

    // March the ray until it hits an element in the scene or the maximum number of steps has been reached,
    // meaning the ray has missed all elements in the scene.
    ClosestElement closest_elem = scene.closest_element(camera.position);
    double ray_len = closest_elem.distance;
    int step = 0;
    for (; step != MaxSteps && closest_elem.distance > Epsilon; ++step) {
        Vec3 ray_pos = ray_dir * ray_len;
        closest_elem = scene.closest_element(ray_pos);
        ray_len += closest_elem.distance;
    }

    if (step == MaxSteps) {
        // Ray missed all elements in the scene.
        return 0xff000000;
    } else {
        // Ray hit an element.
        return 0xffffffff;
    }
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

    // Create the scene to render
    Scene scene;
    scene.add_element(std::make_unique<Sphere>(
        Vec3{ 0, 0, 30 },   // center
        20                  // radius
    ));

    // Create the camera
    Camera camera {
        .position = { 0, 0, -20 },
        .forward  = { 0, 0, 1  },
        .right    = { 1, 0, 0  },
        .up       = { 0, 0, 1  },
        .fov_x    = 60 * std::numbers::pi / 180,
        .fov_y    = 34 * std::numbers::pi / 180
    };

    // TODO: Only cast rays in the camera's view!

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
                pixels[y * WIDTH + x] = raymarch_pixel(scene, camera, x - WIDTH/2, y - HEIGHT/2);
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