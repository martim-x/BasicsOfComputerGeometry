#include "SphereLightingCore.h"

#include <algorithm>
#include <cmath>

namespace lab10 {
namespace {

constexpr double pi = 3.14159265358979323846;
constexpr double epsilon = 1e-12;

Vec3 subtract(const Vec3& a, const Vec3& b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vec3 scale(const Vec3& value, double factor)
{
    return {value.x * factor, value.y * factor, value.z * factor};
}

double dot(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
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

bool angle_count(double span, double step, std::size_t& count)
{
    if (!std::isfinite(step) || step <= 0.0) {
        return false;
    }
    const double intervals = span / step;
    const double rounded = std::round(intervals);
    if (!std::isfinite(intervals) || rounded < 1.0 ||
        std::abs(intervals - rounded) > epsilon * std::max(1.0, intervals)) {
        return false;
    }
    count = static_cast<std::size_t>(rounded);
    return true;
}

std::uint8_t channel(double intensity, std::uint8_t value)
{
    const double result = std::clamp(intensity * static_cast<double>(value), 0.0, 255.0);
    return static_cast<std::uint8_t>(std::lround(result));
}

double clamp_coefficient(double value)
{
    return std::isfinite(value) ? std::clamp(value, 0.0, 1.0) : 0.0;
}

}

std::vector<SphereSample> SampleSphere(double radius, double delta_phi_degrees,
                                       double delta_theta_degrees)
{
    std::size_t phi_intervals = 0;
    std::size_t theta_intervals = 0;
    if (!std::isfinite(radius) || radius <= 0.0 ||
        !angle_count(360.0, delta_phi_degrees, phi_intervals) ||
        !angle_count(180.0, delta_theta_degrees, theta_intervals)) {
        return {};
    }

    std::vector<SphereSample> samples;
    samples.reserve(phi_intervals * (theta_intervals - 1) + 2);
    for (std::size_t ti = 0; ti <= theta_intervals; ++ti) {
        const double theta = (180.0 * static_cast<double>(ti) /
                              static_cast<double>(theta_intervals)) * pi / 180.0;
        const std::size_t phi_count = (ti == 0 || ti == theta_intervals) ? 1 : phi_intervals;
        for (std::size_t fi = 0; fi < phi_count; ++fi) {
            const double phi = (360.0 * static_cast<double>(fi) /
                                static_cast<double>(phi_intervals)) * pi / 180.0;
            const Vec3 position{radius * std::sin(theta) * std::cos(phi),
                                radius * std::sin(theta) * std::sin(phi),
                                radius * std::cos(theta)};
            samples.push_back({position, normalized(position)});
        }
    }
    return samples;
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

bool IsVisible(const SphereSample& sample, const Vec3& camera_position)
{
    const Vec3 view = normalized(subtract(camera_position, sample.position));
    return length(view) > epsilon && dot(normalized(sample.normal), view) > 0.0;
}

double DiffuseIntensity(const SphereSample& sample, const Vec3& light_position,
                        double diffuse_coefficient)
{
    const Vec3 normal = normalized(sample.normal);
    const Vec3 light_direction = normalized(subtract(light_position, sample.position));
    if (length(normal) <= epsilon || length(light_direction) <= epsilon) {
        return 0.0;
    }
    return clamp_coefficient(diffuse_coefficient) *
           std::clamp(dot(normal, light_direction), 0.0, 1.0);
}

double SpecularIntensity(const SphereSample& sample, const Vec3& light_position,
                         const Vec3& camera_position, double specular_coefficient,
                         double shininess)
{
    const Vec3 normal = normalized(sample.normal);
    const Vec3 light_direction = normalized(subtract(light_position, sample.position));
    const Vec3 view_direction = normalized(subtract(camera_position, sample.position));
    if (length(normal) <= epsilon || length(light_direction) <= epsilon ||
        length(view_direction) <= epsilon || !std::isfinite(shininess) || shininess <= 0.0) {
        return 0.0;
    }
    const double normal_light = dot(normal, light_direction);
    if (normal_light <= 0.0) {
        return 0.0;
    }
    const Vec3 reflected = subtract(scale(normal, 2.0 * normal_light), light_direction);
    const double cosine = std::clamp(dot(normalized(reflected), view_direction), 0.0, 1.0);
    return clamp_coefficient(specular_coefficient) * std::pow(cosine, shininess);
}

ColorRGB Shade(const SphereSample& sample, const Vec3& camera_position,
               const Vec3& light_position, ColorRGB source_color, LightingModel model,
               double diffuse_coefficient, double specular_coefficient, double shininess)
{
    const double intensity = model == LightingModel::Diffuse
        ? DiffuseIntensity(sample, light_position, diffuse_coefficient)
        : SpecularIntensity(sample, light_position, camera_position,
                            specular_coefficient, shininess);
    return {channel(intensity, source_color.r),
            channel(intensity, source_color.g),
            channel(intensity, source_color.b)};
}

}
