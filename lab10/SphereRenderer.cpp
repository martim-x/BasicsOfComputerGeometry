#include "SphereRenderer.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace {

struct ScreenSample {
    int x;
    int y;
    double depth;
    lab10::ColorRGB color;
};

double dot(const lab10::Vec3& a, const lab10::Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

}

void DrawLightSphere(CDC& dc, double Radius, CMatrix& PView, CMatrix& PSourceLight,
                     CRect RW, COLORREF Color, int Index)
{
    lab10::LightingModel model;
    if (Index == 0) {
        model = lab10::LightingModel::Diffuse;
    } else if (Index == 1) {
        model = lab10::LightingModel::Specular;
    } else {
        return;
    }
    if (Radius <= 0.0 || RW.Width() < 20 || RW.Height() < 20) {
        return;
    }

    const lab10::Vec3 camera = lab10::SphericalToCartesian(PView(0), PView(1), PView(2));
    const lab10::Vec3 source = lab10::SphericalToCartesian(PSourceLight(0), PSourceLight(1), PSourceLight(2));
    const lab10::Vec3 camera_direction = lab10::SphericalToCartesian(1.0, PView(1), PView(2));
    const lab10::ColorRGB source_color{GetRValue(Color), GetGValue(Color), GetBValue(Color)};
    const int smaller_window_side = (std::min)(RW.Width(), RW.Height());
    const double pixel_radius = (static_cast<double>(smaller_window_side) * 0.5 - 24.0) / Radius;
    if (pixel_radius <= 0.0) {
        return;
    }
    const double center_x = (RW.left + RW.right) * 0.5;
    const double center_y = (RW.top + RW.bottom) * 0.5;
    const std::vector<lab10::SphereSample> samples = lab10::SampleSphere(Radius, 2.0, 2.0);
    std::vector<ScreenSample> visible;
    visible.reserve(samples.size() / 2);
    for (const lab10::SphereSample& sample : samples) {
        if (!lab10::IsVisible(sample, camera)) {
            continue;
        }
        const lab10::ProjectedPoint projected = lab10::ProjectPoint(sample.position, PView(1), PView(2));
        const int x = static_cast<int>(std::lround(center_x + projected.x * pixel_radius));
        const int y = static_cast<int>(std::lround(center_y - projected.y * pixel_radius));
        if (!RW.PtInRect(CPoint(x, y))) {
            continue;
        }
        const lab10::ColorRGB shaded = lab10::Shade(sample, camera, source, source_color, model,
                                                     0.8, 0.8, 24.0);
        visible.push_back({x, y, dot(sample.position, camera_direction), shaded});
    }
    std::stable_sort(visible.begin(), visible.end(), [](const ScreenSample& a, const ScreenSample& b) {
        return a.depth < b.depth;
    });

    constexpr int dot_radius = 2;
    for (const ScreenSample& sample : visible) {
        const COLORREF color = RGB(sample.color.r, sample.color.g, sample.color.b);
        for (int dy = -dot_radius; dy <= dot_radius; ++dy) {
            for (int dx = -dot_radius; dx <= dot_radius; ++dx) {
                if (dx * dx + dy * dy <= dot_radius * dot_radius) {
                    const CPoint pixel(sample.x + dx, sample.y + dy);
                    if (RW.PtInRect(pixel)) {
                        dc.SetPixelV(pixel.x, pixel.y, color);
                    }
                }
            }
        }
    }
}
