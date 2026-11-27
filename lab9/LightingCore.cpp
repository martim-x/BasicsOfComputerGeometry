#include "LightingCore.h"

#include <algorithm>
#include <cmath>

namespace lab9 {
namespace {

constexpr double pi = 3.14159265358979323846;
constexpr double epsilon = 1e-12;

Vec3 subtract(const Vec3& a, const Vec3& b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vec3 add(const Vec3& a, const Vec3& b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec3 scale(const Vec3& value, double factor)
{
    return {value.x * factor, value.y * factor, value.z * factor};
}

double dot(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 cross(const Vec3& a, const Vec3& b)
{
    return {a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
}

double length(const Vec3& value)
{
    return std::sqrt(dot(value, value));
}

Vec3 normalized(const Vec3& value)
{
    const double magnitude = length(value);
    if (!std::isfinite(magnitude) || magnitude <= epsilon) {
        return {0.0, 0.0, 0.0};
    }
    return scale(value, 1.0 / magnitude);
}

Face make_face(const std::array<Vec3, 4>& vertices, std::size_t count, const Vec3& object_center)
{
    Vec3 center{0.0, 0.0, 0.0};
    for (std::size_t i = 0; i < count; ++i) {
        center = add(center, vertices[i]);
    }
    center = scale(center, 1.0 / static_cast<double>(count));
    Vec3 normal = cross(subtract(vertices[1], vertices[0]), subtract(vertices[2], vertices[0]));
    if (dot(normal, subtract(center, object_center)) < 0.0) {
        normal = scale(normal, -1.0);
    }
    return {vertices, count, normal, center};
}

std::uint8_t channel(double intensity, std::uint8_t value)
{
    const double result = std::clamp(intensity * static_cast<double>(value), 0.0, 255.0);
    return static_cast<std::uint8_t>(std::lround(result));
}

}

std::vector<Face> CreateTruncatedPyramid()
{
    const Vec3 a{3.0, 0.0, 0.0};
    const Vec3 b{0.0, 0.0, 0.0};
    const Vec3 c{0.0, 3.0, 0.0};
    const Vec3 a1{1.0, 0.0, 3.0};
    const Vec3 b1{0.0, 0.0, 3.0};
    const Vec3 c1{0.0, 1.0, 3.0};
    const Vec3 object_center{2.0 / 3.0, 2.0 / 3.0, 1.5};

    return {
        make_face({a, b, c, a}, 3, object_center),
        make_face({a1, b1, c1, a1}, 3, object_center),
        make_face({a, a1, b1, b}, 4, object_center),
        make_face({b, b1, c1, c}, 4, object_center),
        make_face({c, c1, a1, a}, 4, object_center)
    };
}

Vec3 SphericalToCartesian(double radius, double azimuth_degrees, double zenith_degrees)
{
    const double azimuth = azimuth_degrees * pi / 180.0;
    const double zenith = zenith_degrees * pi / 180.0;
    return {radius * std::sin(zenith) * std::cos(azimuth),
            radius * std::sin(zenith) * std::sin(azimuth),
            radius * std::cos(zenith)};
}

ProjectedPoint ProjectPoint(const Vec3& point, double azimuth_degrees, double zenith_degrees)
{
    const double azimuth = azimuth_degrees * pi / 180.0;
    const double zenith = zenith_degrees * pi / 180.0;
    const Vec3 right{-std::sin(azimuth), std::cos(azimuth), 0.0};
    const Vec3 up{-std::cos(zenith) * std::cos(azimuth),
                  -std::cos(zenith) * std::sin(azimuth),
                  std::sin(zenith)};
    return {dot(point, right), dot(point, up)};
}

bool IsVisible(const Face& face, const Vec3& camera_position)
{
    if (length(face.normal) <= epsilon) {
        return false;
    }
    return dot(face.normal, subtract(camera_position, face.center)) > 0.0;
}

double DiffuseIntensity(const Face& face, const Vec3& light_position, double diffuse_coefficient)
{
    const Vec3 normal = normalized(face.normal);
    const Vec3 light_direction = normalized(subtract(light_position, face.center));
    if (length(normal) <= epsilon || length(light_direction) <= epsilon ||
        !std::isfinite(diffuse_coefficient)) {
        return 0.0;
    }
    const double coefficient = std::clamp(diffuse_coefficient, 0.0, 1.0);
    return coefficient * std::clamp(dot(normal, light_direction), 0.0, 1.0);
}

ColorRGB Shade(const Face& face, const Vec3& light_position, ColorRGB source_color,
               double diffuse_coefficient)
{
    const double intensity = DiffuseIntensity(face, light_position, diffuse_coefficient);
    return {channel(intensity, source_color.r),
            channel(intensity, source_color.g),
            channel(intensity, source_color.b)};
}

}
