#include "pch.h"

#include <SDL3/SDL.h>

#include "common_defs.h"
#include "vec3.h"
#include "camera.h"
#include "scene.h"
#include "sphere.h"

constexpr RectF kOutputSize = {
    .width = 800,
    .height = 600
};
constexpr double kOutputAspectRatio = kOutputSize.width / kOutputSize.height;

Fov get_adjusted_fov(const Camera& camera) {
    Fov fov = camera.fov();
    double sensor_aspect_ratio = camera.sensor_aspect_ratio();
    SensorFit sensor_fit = camera.sensor_fit();

    switch (sensor_fit) {
        case SensorFit::Fill:
            if (sensor_aspect_ratio > kOutputAspectRatio) {
                fov.horiz *= kOutputAspectRatio / sensor_aspect_ratio;
            } else if (sensor_aspect_ratio < kOutputAspectRatio) {
                fov.vert *= sensor_aspect_ratio / kOutputAspectRatio;
            }
            break;
        case SensorFit::Overscan:
            if (sensor_aspect_ratio > kOutputAspectRatio) {
                fov.vert *= sensor_aspect_ratio / kOutputAspectRatio;
            } else if (sensor_aspect_ratio < kOutputAspectRatio) {
                fov.horiz *= kOutputAspectRatio / sensor_aspect_ratio;
            }
            break;
        default:
            std::terminate();
    }

    return fov;
}

/**
 * Raymarch the specified pixel to determine its color.
 * 
 * @param px      The pixel's x coordinate.
 * @param py      The pixel's y coordinate.
 * @param camera  The camera.
 * @param fov     The final FOV, adjusted for an aspect ratio mismatch between the raster and the sensor.
 * @param scene   The scene holding the elements to render.
 * 
 * @return Returns the pixel's color in ARGB8888 format.
 */
Result<uint32_t, void> raymarch_pixel(int px, int py, const Camera& camera, Fov fov, const Scene& scene)
{
    // A ray that has marched further than this distance is defined to have missed all elements in the scene.
    static constexpr double kMaxDist = 1000;
    // A ray that has been marched this many steps is defined to have missed all elements in the scene.
    static constexpr int kMaxSteps = 10;
    // A ray has hit an element in the scene if its distance to that element is less than Epsilon.
    static constexpr double kEpsilon = 0.001;

    // NOTE: The raster is sampled from the portion of the scene visible on the virtual canvas.

    // NOTE: Graphics APIs follow the convention used in the calculations that follow.
    //       scratchapixel.com follows a different convention:
    //         pipeline: raster space -> NDC space -> screen space, where
    //         NDC space has its origin at the bottom-left and coordinates are in the range [0, 1]
    //         screen space has its origin at the center and coordinates are in the range [-1, 1]
    //       Both conventions arrive at the same result.

    double log_x = kOutputSize.width / 2;
    double log_y = kOutputSize.height / 2;

    // Convert from raster coordinates to Normalized Screen Coordinates (NSC).
    // NSC space has its origin at the top-left and coordinates are in the range [0, 1].
    double u = (px + 0.5) / kOutputSize.width;
    double v = (py + 0.5) / kOutputSize.height;

    #ifdef LOG
    if (px == log_x && py == log_y) {
        std::cout << "u = " << u << ", v = " << v << "\n";
    }
    #endif

    // Convert from NSC to Normalized Device Coordinates (NDC).
    // NDC space has its origin at the center and coordinates are in the range [-1, 1].
    double x_ndc = 2 * u - 1;
    double y_ndc = 1 - 2 * v;

    #ifdef LOG
    if (px == log_x && py == log_y) {
        std::cout << "x_ndc = " << x_ndc << ", y_ndc = " << y_ndc << "\n";
    }
    #endif

    // Calculate the ray direction angles, relative to the direction the camera is facing.
    double ray_dir_horiz = x_ndc * (fov.horiz / 2);
    double ray_dir_vert  = y_ndc * (fov.vert  / 2);

    #ifdef LOG
    if (px == log_x && py == log_y) {
        std::cout << "Ray direction (relative to camera), horizontal = "
            << rad2deg(ray_dir_horiz) << ", vertical = " << rad2deg(ray_dir_vert) << "\n";
    }
    #endif

    // Calculate the ray direction vector, in camera coordinates.
    double xz_plane_len = std::cos(ray_dir_vert);
    double ray_dir_x = xz_plane_len * std::sin(ray_dir_horiz);
    double ray_dir_y = std::sin(ray_dir_vert);
    // NOTE: The z coordinate is explicitly negated because the camera points along the -z axis.
    double ray_dir_z = -xz_plane_len * std::cos(ray_dir_horiz);
    Vec3 ray_dir_cam = Vec3 { ray_dir_x, ray_dir_y, ray_dir_z };

    #ifdef LOG
    if (px == log_x && py == log_y) {
        std::cout << "Ray direction, camera coords, x = "
            << ray_dir_cam.x << ", y = " << ray_dir_cam.y << ", z = " << ray_dir_cam.z << "\n";
    }
    #endif

    // Convert the ray direction vector into world coordinates.
    Vec3 ray_dir = camera.basis() * ray_dir_cam;

    #ifdef LOG
    if (px == log_x && py == log_y) {
        std::cout << "Ray direction, world coords, x = "
            << ray_dir.x << ", y = " << ray_dir.y << ", z = " << ray_dir.z << "\n";
    }
    #endif

    // March the ray until it hits an object in the scene or misses all elements
    Vec3 current_position = camera.position();
    Result<ClosestObject, void> closest_obj = scene.closest_object(camera.position());
    if (!closest_obj.is_ok()) {
        return Result<uint32_t, void>::err();
    }

    #ifdef LOG
    if (px == log_x && py == log_y) {
        std::cout << "Object closest to camera is " << closest_obj.value().distance << "m away\n";
    }
    #endif

    double ray_len = closest_obj.value().distance;
    for (int step = 0; step != kMaxSteps && ray_len <= kMaxDist; ++step) {
        current_position = camera.position() + ray_dir * ray_len;

        #ifdef LOG
        if (px == log_x && py == log_y) {
            std::cout << "Current position, world coords, x = "
                << current_position.x << ", y = " << current_position.y << ", z = "
                << current_position.z << "\n";
        }
        #endif

        closest_obj = scene.closest_object(current_position);
        if (!closest_obj.is_ok()) {
            return Result<uint32_t, void>::err();
        }

        #ifdef LOG
        if (px == log_x && py == log_y) {
            std::cout << "Closest object to position is " << closest_obj.value().distance << "m away\n";
        }
        #endif
        
        if (closest_obj.value().distance < kEpsilon) {
            // Ray hit an object.

            #ifdef LOG
            if (px == log_x && py == log_y) {
                std::cout << "Ray hit an object!\n";
            }
            #endif

            // Determine if the point should be visible by converting it back into camera coordinates
            // and checking it against the near/far clipping planes.
            Vec3 cur_pos_cam = camera.world_to_camera().rotation * current_position +
                camera.world_to_camera().translation;

            #ifdef LOG
            if (px == log_x && py == log_y) {
                std::cout << "Current position, camera coords, x = " << cur_pos_cam.x
                    << ", y = " << cur_pos_cam.y << ", z = " << cur_pos_cam.z << "\n";
            }
            #endif

            // Negate the z coordinate to get the point's distance from the camera.
            double dist_z = -cur_pos_cam.z;
            if (dist_z >= camera.clip_near() && dist_z <= camera.clip_far()) {
                #ifdef LOG
                if (px == log_x && py == log_y) {
                    std::cout << "Object inside clipping planes: visible!\n";
                }
                #endif
                return Result<uint32_t, void>::ok(0xffff0000);
            } else {
                #ifdef LOG
                if (px == log_x && py == log_y) {
                    std::cout << "Object outside clipping planes: NOT visible!\n";
                }
                #endif
                return Result<uint32_t, void>::ok(0xff000000);
            }
        }

        ray_len += closest_obj.value().distance;
    }

    // Ray missed all objects in the scene.
    #ifdef LOG
    if (px == log_x && py == log_y) {
        std::cout << "Ray missed all objects!\n";
    }
    #endif

    return Result<uint32_t, void>::ok(0xff000000);
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
        "Raymarching Demo",
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
    std::unique_ptr<uint32_t[]> pixels = std::make_unique<uint32_t[]>(kOutputSize.width * kOutputSize.height);

    // Create the camera.
    double sensor_aspect_ratio = 3.0 / 2;
    double fov_horiz = 60;
    double clip_near = 0.1;
    double clip_far = 100;
    Vec3 cam_pos = { 0, 0, 0 };
    double cam_rot_x = 0;
    double cam_rot_y = 0;
    double cam_rot_z = 0;
    Camera camera {
        sensor_aspect_ratio, fov_horiz, SensorFit::Overscan, clip_near, clip_far,
        cam_pos, cam_rot_x, cam_rot_y, cam_rot_z
     };

     // Adjust the FOV for an aspect ratio mismatch between the raster and the sensor.
    Fov fov = get_adjusted_fov(camera);

    #ifdef LOG
    std::cout << "Adjusted FOV, horizontal = " << rad2deg(fov.horiz)
        << " deg, vertical = " << rad2deg(fov.vert) << "\n";
    #endif

    // Create the scene to render.
    // NOTE: The camera is facing along the negative z-axis.
    //       If an object is to be visible, its z coordinate must be negative.
    std::array<Object, MaxObjects> scene_objects = {
        Sphere {
            Vec3 { 0, 0, -30 }, // center
            10                  // radius
        }
    };
    Scene scene{scene_objects};

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

        // Compute pixels on CPU.
        for (int y = 0; y < kOutputSize.height; y++) {
            for (int x = 0; x < kOutputSize.width; x++) {
                Result<uint32_t, void> px_color = raymarch_pixel(x, y, camera, fov, scene);
                if (!px_color.is_ok()) {
                    std::cerr << "ERROR: raymarching failed" << std::endl;
                    break;
                }
                pixels[y * kOutputSize.width + x] = px_color.value();
            }
        }

        // Update SDL texture with pixel buffer
        SDL_UpdateTexture(texture, nullptr, pixels.get(), kOutputSize.width * sizeof(uint32_t));

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