#include "pch.h"

#include <SDL3/SDL.h>

#include "common_defs.h"
#include "vec3.h"
#include "camera.h"
#include "scene.h"
#include "sphere.h"
#include "cube.h"
#include "box2d.h"

constexpr RectI kOutputSize = {
    .width = 1024,
    .height = 768
};
constexpr double kOutputAspectRatio = kOutputSize.width / kOutputSize.height;

Fov get_adjusted_fov(const Camera& camera) {
    Fov fov = camera.fov();
    double sensor_aspect_ratio = camera.sensor_aspect_ratio();
    SensorFit sensor_fit = camera.sensor_fit();

    switch (sensor_fit) {
        case SensorFit::Fill:
            if (sensor_aspect_ratio > kOutputAspectRatio) {
                fov.horiz = 2 * std::atan(std::tan(fov.vert * 0.5) * kOutputAspectRatio);
            } else if (sensor_aspect_ratio < kOutputAspectRatio) {
                fov.vert = 2 * std::atan(std::tan(fov.horiz * 0.5) / kOutputAspectRatio);
            }
            break;
        case SensorFit::Overscan:
            if (sensor_aspect_ratio > kOutputAspectRatio) {
                fov.vert = 2 * std::atan(std::tan(fov.horiz * 0.5) / kOutputAspectRatio);
            } else if (sensor_aspect_ratio < kOutputAspectRatio) {
                fov.horiz = 2 * std::atan(std::tan(fov.vert * 0.5) * kOutputAspectRatio);
            }
            break;
        default:
            std::cerr << "ERROR: invalid SensorFit method\n";
            std::terminate();
    }

    return fov;
}

std::optional<uint32_t> raymarch_pixel(int px, int py, const Camera& camera, Fov fov, const Scene& scene)
{
    // A ray that has marched further than this distance is defined to have missed all objects in the scene.
    static constexpr double kMaxDist = 200;
    // A ray that has been marched this many steps is defined to have missed all objects in the scene.
    static constexpr int kMaxSteps = 40;
    // A ray has hit an object in the scene if its distance to that object is less than Epsilon.
    static constexpr double kEpsilon = 0.001;

    // NOTE: The raster is sampled from the portion of the scene visible on the virtual canvas.

    // NOTE: Graphics APIs follow the convention used in the calculations that follow.
    //       scratchapixel.com follows a different convention:
    //         pipeline: raster space -> NDC space -> screen space, where
    //         NDC space has its origin at the bottom-left and coordinates are in the range [0, 1]
    //         screen space has its origin at the center and coordinates are in the range [-1, 1]
    //       Both conventions arrive at the same result.

    // Convert from raster coordinates to Normalized Screen Coordinates (NSC).
    // NSC space has its origin at the top-left and coordinates are in the range [0, 1].
    double u = (px + 0.5) / kOutputSize.width;
    double v = (py + 0.5) / kOutputSize.height;

    // Convert from NSC to Normalized Device Coordinates (NDC).
    // NDC space has its origin at the center and coordinates are in the range [-1, 1].
    double x_ndc = 2 * u - 1;
    double y_ndc = 1 - 2 * v;

    // Apply the NDC coordinates to the FOV to get a ray direction vector in camera space.
    // Assume the virtual canvas is 1 unit away from the camera along the z-axis (simplifies tan).
    double x_cam = x_ndc * std::tan(fov.horiz * 0.5);
    double y_cam = y_ndc * std::tan(fov.vert  * 0.5);
    double z_cam = -1;

    // Normalize the ray direction vector.
    Vec3 ray_dir_cam = Vec3 { x_cam, y_cam, z_cam }.normalize();

    // Convert the ray direction vector into world coordinates.
    Vec3 ray_dir = camera.basis() * ray_dir_cam;

    // Use the distance to the closest object as the initial ray length.
    Vec3 current_position = camera.position();
    auto closest_obj = scene.closest_object(camera.position());
    if (!closest_obj) {
        return std::nullopt;
    }
    double ray_len = closest_obj.value().distance;

    // Return a black pixel if the ray doesn't hit an object.
    uint32_t pixel_argb = 0xff000000;

    // March the ray until it hits an object in the scene or misses all objects.
    for (int step = 0; step != kMaxSteps && ray_len <= kMaxDist; ++step) {
        current_position = camera.position() + ray_dir * ray_len;


        closest_obj = scene.closest_object(current_position);
        if (!closest_obj) {
            return std::nullopt;
        }
        
        if (closest_obj.value().distance < kEpsilon) {
            // Ray hit an object.

            // Determine if the point should be visible by converting it back into camera coordinates
            // and checking it against the near/far clipping planes.
            Vec3 cur_pos_cam = camera.world_to_camera().rotation * current_position +
                camera.world_to_camera().translation;

            // Negate the z coordinate to get the point's distance from the camera.
            double dist_z = -cur_pos_cam.z;
            if (dist_z >= camera.clip_near() && dist_z <= camera.clip_far()) {
                // The position should be visible. Return a color of red.
                pixel_argb = 0xffff0000;
            }

            break;
        }

        ray_len += closest_obj.value().distance;
    }

    return std::make_optional(pixel_argb);
}

int main(int argc, char** argv)
{
    // ----------------------------------------------------------------------
    // Initialize SDL and create the window.
    // ----------------------------------------------------------------------
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "CPU Raymarcher",
        kOutputSize.width, kOutputSize.height,
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
        kOutputSize.width, kOutputSize.height
    );
    if (!texture) {
        std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << "\n";
        return 1;
    }

    // Allocate the CPU pixel buffer.
    std::vector<uint32_t> pixels(kOutputSize.width * kOutputSize.height);

    // Create the camera.
    double sensor_aspect_ratio = 3.0 / 2;
    double fov_horiz = 80;
    double clip_near = 0.1;
    double clip_far = 100;
    Vec3 cam_pos { 0, 0, 0 };
    EulerAngles cam_rot { 0, 0, 0 };
    Camera camera {
        sensor_aspect_ratio, fov_horiz, SensorFit::Overscan, clip_near, clip_far,
        cam_pos, cam_rot
    };

     // Adjust the FOV for an aspect ratio mismatch between the raster and the sensor.
    Fov fov = get_adjusted_fov(camera);

    // Create the scene to render.
    // NOTE: The camera is facing along the negative z-axis.
    //       If an object is to be visible, its z coordinate must be negative.
    std::array<Object, MaxObjects> scene_objects = {
        Sphere {
            Vec3 { -15, 0, -30 },
            5
        },
        Cube {
            Vec3 { 0, 0, -30 },
            10,
            EulerAngles { 0, 45, 0 }
        },
        Box2D {
            Vec3 { 15, 0, -30 },
            { 10, 10 },
            EulerAngles { 0, 45, 0 }
        }
    };
    Scene scene { scene_objects };

    // ----------------------------------------------------------------------
    // Main Loop.
    // ----------------------------------------------------------------------
    SDL_Event event;
    bool done = false;
    while (true) {
        // Handle events.
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }
        if (done) {
            break;
        }

        // Compute pixels on the CPU.
        for (int y = 0; y < kOutputSize.height; y++) {
            for (int x = 0; x < kOutputSize.width; x++) {
                auto px_color = raymarch_pixel(x, y, camera, fov, scene);
                if (!px_color) {
                    std::cerr << "ERROR: raymarching failed\n";
                    return 1;
                }
                pixels[y * kOutputSize.width + x] = px_color.value();
            }
        }

        // Update SDL texture with pixel buffer
        SDL_UpdateTexture(texture, nullptr, pixels.data(), kOutputSize.width * sizeof(uint32_t));

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