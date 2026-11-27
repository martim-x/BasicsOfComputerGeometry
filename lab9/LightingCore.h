#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace lab9 {

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

struct Face {
    std::array<Vec3, 4> vertices;
    std::size_t vertex_count;
    Vec3 normal;
    Vec3 center;
};

std::vector<Face> CreateTruncatedPyramid();
Vec3 SphericalToCartesian(double radius, double azimuth_degrees, double zenith_degrees);
ProjectedPoint ProjectPoint(const Vec3& point, double azimuth_degrees, double zenith_degrees);
bool IsVisible(const Face& face, const Vec3& camera_position);
double DiffuseIntensity(const Face& face, const Vec3& light_position, double diffuse_coefficient);
ColorRGB Shade(const Face& face, const Vec3& light_position, ColorRGB source_color,
               double diffuse_coefficient);

}
