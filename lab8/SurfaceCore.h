#pragma once

#include <iosfwd>
#include <vector>

namespace lab8 {

enum class SurfaceKind {
    EllipticParaboloid,
    HyperbolicParaboloid,
    UpperHemisphere
};

struct Camera {
    static constexpr double radius = 10.0;
    double azimuth = 45.0;
    double zenith = 45.0;
};

bool ReadCameraAngles(std::istream& input, Camera& camera);

struct Vec3 {
    double x;
    double y;
    double z;
};

struct ProjectedPoint {
    double x;
    double y;
};

struct Triangle {
    Vec3 p0;
    Vec3 p1;
    Vec3 p2;
    double depth;
};

bool EvaluateSurface(SurfaceKind kind, double x, double y, double& z);
std::vector<Triangle> BuildSurface(SurfaceKind kind, double x_min, double x_max,
                                   double y_min, double y_max, double step);
Vec3 CameraDirection(double azimuth_degrees, double zenith_degrees);
ProjectedPoint ProjectPoint(const Vec3& point, double azimuth_degrees,
                            double zenith_degrees);
void SortFarToNear(std::vector<Triangle>& triangles, const Vec3& camera_direction);

}
