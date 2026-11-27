#include "SurfaceCore.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace lab8 {
namespace {

constexpr double pi = 3.14159265358979323846;
constexpr double sphere_radius = 3.0;
constexpr double domain_epsilon = 1e-12;

bool finite(double value)
{
    return std::isfinite(value);
}

Vec3 add(const Vec3& a, const Vec3& b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec3 scale(const Vec3& point, double factor)
{
    return {point.x * factor, point.y * factor, point.z * factor};
}

double dot(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 centroid(const Triangle& triangle)
{
    return scale(add(add(triangle.p0, triangle.p1), triangle.p2), 1.0 / 3.0);
}

bool grid_count(double minimum, double maximum, double step, std::size_t& count)
{
    if (!finite(minimum) || !finite(maximum) || !finite(step) || step <= 0.0 || maximum < minimum) {
        return false;
    }
    const double intervals = (maximum - minimum) / step;
    if (!finite(intervals) || intervals > 100000.0) {
        return false;
    }
    const double rounded = std::round(intervals);
    if (std::abs(intervals - rounded) > domain_epsilon * std::max(1.0, intervals)) {
        return false;
    }
    count = static_cast<std::size_t>(rounded);
    return count > 0;
}

}

bool EvaluateSurface(SurfaceKind kind, double x, double y, double& z)
{
    if (!finite(x) || !finite(y)) {
        return false;
    }
    switch (kind) {
    case SurfaceKind::EllipticParaboloid:
        z = x * x + y * y;
        return finite(z);
    case SurfaceKind::HyperbolicParaboloid:
        z = x * x - y * y;
        return finite(z);
    case SurfaceKind::UpperHemisphere: {
        const double radicand = sphere_radius * sphere_radius - x * x - y * y;
        if (radicand < -domain_epsilon) {
            return false;
        }
        z = std::sqrt(std::max(0.0, radicand));
        return finite(z);
    }
    }
    return false;
}

std::vector<Triangle> BuildSurface(SurfaceKind kind, double x_min, double x_max,
                                   double y_min, double y_max, double step)
{
    std::size_t x_intervals = 0;
    std::size_t y_intervals = 0;
    if (!grid_count(x_min, x_max, step, x_intervals) ||
        !grid_count(y_min, y_max, step, y_intervals)) {
        return {};
    }

    struct Sample {
        Vec3 point{};
        bool valid = false;
    };

    std::vector<std::vector<Sample>> grid(x_intervals + 1,
                                          std::vector<Sample>(y_intervals + 1));
    for (std::size_t i = 0; i <= x_intervals; ++i) {
        const double x = x_min + static_cast<double>(i) * step;
        for (std::size_t j = 0; j <= y_intervals; ++j) {
            const double y = y_min + static_cast<double>(j) * step;
            double z = 0.0;
            if (EvaluateSurface(kind, x, y, z)) {
                grid[i][j] = {{x, y, z}, true};
            }
        }
    }

    std::vector<Triangle> triangles;
    for (std::size_t i = 0; i < x_intervals; ++i) {
        for (std::size_t j = 0; j < y_intervals; ++j) {
            const Sample& p00 = grid[i][j];
            const Sample& p10 = grid[i + 1][j];
            const Sample& p11 = grid[i + 1][j + 1];
            const Sample& p01 = grid[i][j + 1];
            if (!(p00.valid && p10.valid && p11.valid && p01.valid)) {
                continue;
            }
            triangles.push_back({p00.point, p10.point, p11.point, 0.0});
            triangles.push_back({p00.point, p11.point, p01.point, 0.0});
        }
    }
    return triangles;
}

Vec3 CameraDirection(double azimuth_degrees, double zenith_degrees)
{
    const double azimuth = azimuth_degrees * pi / 180.0;
    const double zenith = zenith_degrees * pi / 180.0;
    return {std::sin(zenith) * std::cos(azimuth),
            std::sin(zenith) * std::sin(azimuth),
            std::cos(zenith)};
}

ProjectedPoint ProjectPoint(const Vec3& point, double azimuth_degrees,
                            double zenith_degrees)
{
    const double azimuth = azimuth_degrees * pi / 180.0;
    const double zenith = zenith_degrees * pi / 180.0;
    const Vec3 right{-std::sin(azimuth), std::cos(azimuth), 0.0};
    const Vec3 up{-std::cos(zenith) * std::cos(azimuth),
                  -std::cos(zenith) * std::sin(azimuth),
                  std::sin(zenith)};
    return {dot(point, right), dot(point, up)};
}

void SortFarToNear(std::vector<Triangle>& triangles, const Vec3& camera_direction)
{
    const double length = std::sqrt(dot(camera_direction, camera_direction));
    if (!finite(length) || length <= std::numeric_limits<double>::epsilon()) {
        return;
    }
    const Vec3 direction = scale(camera_direction, 1.0 / length);
    for (Triangle& triangle : triangles) {
        triangle.depth = dot(centroid(triangle), direction);
    }
    std::stable_sort(triangles.begin(), triangles.end(),
                     [](const Triangle& a, const Triangle& b) {
                         return a.depth < b.depth;
                     });
}

}
