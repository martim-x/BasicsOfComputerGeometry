#include "../SphereLightingCore.h"
#include <cassert>
#include <cmath>

using lab10::ColorRGB;
using lab10::LightingModel;
using lab10::SphereSample;
using lab10::Vec3;

static bool near(double a, double b, double eps = 1e-9)
{
    return std::abs(a - b) <= eps;
}

int main()
{
    const auto samples = lab10::SampleSphere(2.0, 2.0, 2.0);
    assert(samples.size() == 16022);
    int north_pole_count = 0;
    int south_pole_count = 0;
    for (const SphereSample& sample : samples) {
        assert(near(sample.position.x * sample.position.x +
                    sample.position.y * sample.position.y +
                    sample.position.z * sample.position.z, 4.0));
        assert(near(sample.normal.x * sample.normal.x +
                    sample.normal.y * sample.normal.y +
                    sample.normal.z * sample.normal.z, 1.0));
        if (near(sample.position.x, 0.0) && near(sample.position.y, 0.0)) {
            if (sample.position.z > 0.0) ++north_pole_count;
            else ++south_pole_count;
        }
    }
    assert(north_pole_count == 1 && south_pole_count == 1);

    const SphereSample top{{0.0, 0.0, 2.0}, {0.0, 0.0, 1.0}};
    const Vec3 camera{0.0, 0.0, 15.0};
    const Vec3 light_above{0.0, 0.0, 10.0};
    const Vec3 light_below{0.0, 0.0, -10.0};
    const ColorRGB yellow{255, 255, 0};
    assert(lab10::IsVisible(top, camera));
    assert(!lab10::IsVisible(top, light_below));
    assert(near(lab10::DiffuseIntensity(top, light_above, 0.8), 0.8));
    assert(near(lab10::DiffuseIntensity(top, light_below, 0.8), 0.0));
    assert(near(lab10::DiffuseIntensity(top, top.position, 0.8), 0.0));
    assert(near(lab10::SpecularIntensity(top, light_above, camera, 0.8, 24.0), 0.8));
    assert(near(lab10::SpecularIntensity(top, light_above, Vec3{10.0, 0.0, 2.0}, 0.8, 24.0), 0.0));
    assert(near(lab10::SpecularIntensity(top, light_below, camera, 0.8, 24.0), 0.0));

    const ColorRGB diffuse = lab10::Shade(top, camera, light_above, yellow,
                                           LightingModel::Diffuse, 0.8, 0.8, 24.0);
    const ColorRGB specular = lab10::Shade(top, camera, light_above, yellow,
                                            LightingModel::Specular, 0.8, 0.8, 24.0);
    assert(diffuse.r == 204 && diffuse.g == 204 && diffuse.b == 0);
    assert(specular.r == 204 && specular.g == 204 && specular.b == 0);
    const Vec3 source = lab10::SphericalToCartesian(10.0, 45.0, 45.0);
    assert(near(source.x, 5.0));
    assert(near(source.y, 5.0));
    assert(near(source.z, std::sqrt(50.0)));
}
