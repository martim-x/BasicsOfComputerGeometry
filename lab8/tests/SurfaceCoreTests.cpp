#include "../SurfaceCore.h"
#include <cassert>
#include <cmath>
#include <sstream>
#include <vector>

using lab8::SurfaceKind;
using lab8::Triangle;
using lab8::Vec3;

static bool near(double a, double b, double eps = 1e-9)
{
    return std::abs(a - b) <= eps;
}

static Triangle at_z(double z)
{
    return {{0.0, 0.0, z}, {1.0, 0.0, z}, {0.0, 1.0, z}, 0.0};
}

static void check_camera_input()
{
    lab8::Camera camera;
    assert(near(camera.radius, 10.0) && near(camera.azimuth, 45.0) && near(camera.zenith, 45.0));
    std::istringstream wrapped("-360.5\n120.25\n");
    assert(lab8::ReadCameraAngles(wrapped, camera));
    assert(near(camera.azimuth, 359.5) && near(camera.zenith, 120.25));

    for (const char* text : {"60", "60 -0.5", "60 180.5", "60 45 extra", "inf 45", "15 nan"}) {
        std::istringstream invalid(text);
        assert(!lab8::ReadCameraAngles(invalid, camera));
        assert(near(camera.azimuth, 359.5) && near(camera.zenith, 120.25));
    }

    std::istringstream large("1e20 180");
    assert(lab8::ReadCameraAngles(large, camera));
    assert(near(camera.azimuth, 280.0) && near(camera.zenith, 180.0));
}

int main()
{
    double z = 0.0;
    assert(lab8::EvaluateSurface(SurfaceKind::EllipticParaboloid, 2.0, 3.0, z));
    assert(near(z, 13.0));
    assert(lab8::EvaluateSurface(SurfaceKind::HyperbolicParaboloid, 2.0, 3.0, z));
    assert(near(z, -5.0));
    assert(lab8::EvaluateSurface(SurfaceKind::UpperHemisphere, 1.0, 2.0, z));
    assert(near(z, 2.0));
    assert(lab8::EvaluateSurface(SurfaceKind::UpperHemisphere, 3.0, 0.0, z));
    assert(near(z, 0.0));
    z = 99.0;
    assert(!lab8::EvaluateSurface(SurfaceKind::UpperHemisphere, 3.1, 0.0, z));
    assert(near(z, 99.0));

    const auto elliptic = lab8::BuildSurface(SurfaceKind::EllipticParaboloid, -5.0, 5.0, -5.0, 5.0, 0.25);
    const auto hyperbolic = lab8::BuildSurface(SurfaceKind::HyperbolicParaboloid, -5.0, 5.0, -5.0, 5.0, 0.25);
    const auto hemisphere = lab8::BuildSurface(SurfaceKind::UpperHemisphere, -3.0, 3.0, -3.0, 3.0, 0.25);
    assert(elliptic.size() == 3200);
    assert(hyperbolic.size() == 3200);
    bool east = false;
    bool west = false;
    bool north = false;
    bool south = false;
    for (const auto& tri : hemisphere) {
        const double area2 = (tri.p1.x - tri.p0.x) * (tri.p2.y - tri.p0.y)
                           - (tri.p1.y - tri.p0.y) * (tri.p2.x - tri.p0.x);
        assert(area2 > 0.0);
        for (const Vec3* p : {&tri.p0, &tri.p1, &tri.p2}) {
            assert(p->x * p->x + p->y * p->y <= 9.0 + 1e-9);
            assert(near(p->x * p->x + p->y * p->y + p->z * p->z, 9.0));
            assert(p->z >= 0.0);
            if (near(p->z, 0.0)) {
                east |= near(p->x, 3.0) && near(p->y, 0.0);
                west |= near(p->x, -3.0) && near(p->y, 0.0);
                north |= near(p->x, 0.0) && near(p->y, 3.0);
                south |= near(p->x, 0.0) && near(p->y, -3.0);
            }
        }
    }
    assert(east && west && north && south);

    std::vector<Triangle> painter{at_z(2.0), at_z(0.0), at_z(1.0)};
    lab8::SortFarToNear(painter, Vec3{0.0, 0.0, 1.0});
    assert(near(painter[0].depth, 0.0));
    assert(near(painter[1].depth, 1.0));
    assert(near(painter[2].depth, 2.0));
    check_camera_input();
}
