#pragma once

#include "vec3.h"
#include "common_defs.h"
#include "util.h"

enum class SensorFit {
    // Decrease the canvas size (FOV) to match the output aspect ratio.
    Fill,
    // Increase the canvas size (FOV) to match the output aspect ratio.
    Overscan
};

struct Fov {
    double horiz;
    double vert;
};

class Camera {
public:
    /**
     * @param fov_horiz    The horizontal FOV, in degrees.
     * @param rot_x        The rotation angle, in degrees, around the x-axis.
     * @param rot_y        The rotation angle, in degrees, around the y-axis.
     * @param rot_z        The rotation angle, in degrees, around the z-axis.
     */
    Camera(double sensor_aspect_ratio, double fov_horiz, SensorFit sensor_fit, double clip_near, double clip_far,
           Vec3 pos, double rot_x, double rot_y, double rot_z) noexcept;

      Fov fov() const noexcept { return fov_; }
      double sensor_aspect_ratio() const noexcept { return sensor_aspect_ratio_; }
      SensorFit sensor_fit() const noexcept { return sensor_fit_; }
      Mat3 direction() const noexcept {
        return { i_hat_, j_hat_, -k_hat_ };
      }
      Vec3 position() const noexcept { return pos_; }
    
private:
    // The focal length and sensor size define the **internal** geometry of the camera.
    // Together they determine the FOV.
    // NOTE: The focal length and sensor size are unnecessary for a raymarcher.
    //       The desired sensor aspect ratio and FOV are specified directly.
    //
    // The focal length, in mm.
    // This is the distance between the aperture (pinhole) and the image plane (film) in a physical pinhole camera.
    // double focal_len_;
    // The sensor size, in inches.
    // This is the film size in an analog camera, or the sensor size in a digital camera.
    // It defines the sensor aspect ratio.
    // RectF sensor_sz_;

    // The ratio of the width to the height of the film/sensor.
    double sensor_aspect_ratio_;
    // The FOV, in radians.
    Fov fov_;
    // How to adjust the canvas size (FOV) to match the output aspect ratio.
    SensorFit sensor_fit_;

    // The near and far clipping planes are imaginary planes located at specific distances from the camera
    // along the camera's line of sight.
    // Only objects located between the camera's two clipping planes are rendered in the camera's view.
    //
    // The distance, in world units, to the near clipping plane.
    // NOTE: It's typical to place the virtual image plane (which contains the canvas) at the near clipping plane.
    double clip_near_;
    // The distance, in world units, to the far clipping plane.
    double clip_far_;

    // The camera's position, expressed in world coordinates.
    Vec3 pos_;

    // The camera's local coordinate system is a right-handed x/y/z system where:
    //   i_hat points right along the x-axis,
    //   j_hat points up along the y-axis,
    //   k_hat points backward along the z-axis, opposite the camera's forward direction.
    // These basis vectors are expressed in world coordinates.
    Vec3 i_hat_;
    Vec3 j_hat_;
    Vec3 k_hat_;    // points backward

    // ***************************************************************************************************
    // *                        Calculated Parameters
    // ***************************************************************************************************

    // The size, in world units, of the canvas.
    // NOTE: This is unnecessary for a raymarcher.
    // RectF canvas_sz_;
};