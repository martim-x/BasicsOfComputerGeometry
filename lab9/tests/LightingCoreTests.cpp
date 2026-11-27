#include "../LightingCore.h"
#include <cassert>
#include <cmath>

using lab9::ColorRGB;
using lab9::Face;
using lab9::Vec3;

static bool near(double a, double b, double eps = 1e-9)
{
    return std::abs(a - b) <= eps;
}

int main()
{
    const auto faces = lab9::CreateTruncatedPyramid();
    assert(faces.size() == 5);
    assert(faces[0].normal.z < 0.0);
    assert(faces[1].normal.z > 0.0);
    assert(lab9::IsVisible(faces[1], Vec3{0.0, 0.0, 10.0}));
    assert(!lab9::IsVisible(faces[0], Vec3{0.0, 0.0, 10.0}));
    assert(lab9::IsVisible(faces[0], Vec3{0.0, 0.0, -10.0}));
    assert(!lab9::IsVisible(faces[1], Vec3{0.0, 0.0, -10.0}));

    Face upward{};
    upward.normal = {0.0, 0.0, 1.0};
    upward.center = {0.0, 0.0, 0.0};
    assert(near(lab9::DiffuseIntensity(upward, Vec3{0.0, 0.0, 10.0}, 0.8), 0.8));
    assert(near(lab9::DiffuseIntensity(upward, Vec3{0.0, 0.0, -10.0}, 0.8), 0.0));
    assert(near(lab9::DiffuseIntensity(upward, Vec3{0.0, 0.0, 0.0}, 0.8), 0.0));

    const ColorRGB white{255, 255, 255};
    const ColorRGB shaded = lab9::Shade(upward, Vec3{0.0, 0.0, 10.0}, white, 0.8);
    assert(shaded.r == 204 && shaded.g == 204 && shaded.b == 204);
    const ColorRGB dark = lab9::Shade(upward, Vec3{0.0, 0.0, -10.0}, white, 0.8);
    assert(dark.r == 0 && dark.g == 0 && dark.b == 0);

    const Vec3 source = lab9::SphericalToCartesian(100.0, 45.0, 45.0);
    assert(near(source.x, 50.0));
    assert(near(source.y, 50.0));
    assert(near(source.z, std::sqrt(5000.0)));
}
