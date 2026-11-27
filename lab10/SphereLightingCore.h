#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace lab10 {

enum class LightingModel {
    Diffuse,
    Specular
};

struct Vec3 {
    double x;
    double y;
    double z;
};

struct ProjectedPoint {
    double x;
    double y;
};

struct ColorRGB {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

struct SphereSample {
    Vec3 position;
    Vec3 normal;
};

std::vector<SphereSample> SampleSphere(double radius, double delta_phi_degrees,
                                       double delta_theta_degrees);
Vec3 SphericalToCartesian(double radius, double azimuth_degrees, double zenith_degrees);
ProjectedPoint ProjectPoint(const Vec3& point, double azimuth_degrees, double zenith_degrees);
bool IsVisible(const SphereSample& sample, const Vec3& camera_position);
double DiffuseIntensity(const SphereSample& sample, const Vec3& light_position,
                        double diffuse_coefficient);
double SpecularIntensity(const SphereSample& sample, const Vec3& light_position,
                         const Vec3& camera_position, double specular_coefficient,
                         double shininess);
ColorRGB Shade(const SphereSample& sample, const Vec3& camera_position,
               const Vec3& light_position, ColorRGB source_color, LightingModel model,
               double diffuse_coefficient, double specular_coefficient, double shininess);

}
