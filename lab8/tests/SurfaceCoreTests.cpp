#include "../SurfaceCore.h"
#include <cassert>
#include <cmath>
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
    assert(hemisphere.size() == 784);
    for (const auto& tri : hemisphere) {
        for (const Vec3* p : {&tri.p0, &tri.p1, &tri.p2}) {
            assert(p->x * p->x + p->y * p->y <= 9.0 + 1e-9);
            assert(near(p->x * p->x + p->y * p->y + p->z * p->z, 9.0));
            assert(p->z >= 0.0);
        }
    }

    std::vector<Triangle> painter{at_z(2.0), at_z(0.0), at_z(1.0)};
    lab8::SortFarToNear(painter, Vec3{0.0, 0.0, 1.0});
    assert(near(painter[0].depth, 0.0));
    assert(near(painter[1].depth, 1.0));
    assert(near(painter[2].depth, 2.0));
}
