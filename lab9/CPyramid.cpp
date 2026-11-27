#include "CPyramid.h"

#include <algorithm>
#include <cmath>
#include <limits>

CPyramid::CPyramid() : faces_(lab9::CreateTruncatedPyramid())
{
}

void CPyramid::ColorDraw(CDC& dc, CMatrix& PView, CMatrix& PLight,
                         CRect& RW, COLORREF Color) const
{
    const lab9::Vec3 camera = lab9::SphericalToCartesian(PView(0), PView(1), PView(2));
    const lab9::Vec3 light = lab9::SphericalToCartesian(PLight(0), PLight(1), PLight(2));
    const lab9::ColorRGB source{GetRValue(Color), GetGValue(Color), GetBValue(Color)};
    std::vector<const lab9::Face*> visible;
    for (const lab9::Face& face : faces_) {
        if (lab9::IsVisible(face, camera)) {
            visible.push_back(&face);
        }
    }
    const double camera_length = std::sqrt(camera.x * camera.x + camera.y * camera.y + camera.z * camera.z);
    if (camera_length <= 1e-12 || visible.empty()) {
        return;
    }
    const lab9::Vec3 camera_direction{camera.x / camera_length, camera.y / camera_length, camera.z / camera_length};
    std::stable_sort(visible.begin(), visible.end(), [&camera_direction](const lab9::Face* a, const lab9::Face* b) {
        const double da = a->center.x * camera_direction.x + a->center.y * camera_direction.y + a->center.z * camera_direction.z;
        const double db = b->center.x * camera_direction.x + b->center.y * camera_direction.y + b->center.z * camera_direction.z;
        return da < db;
    });

    double min_x = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::lowest();
    double min_y = std::numeric_limits<double>::max();
    double max_y = std::numeric_limits<double>::lowest();
    for (const lab9::Face& face : faces_) {
        for (std::size_t i = 0; i < face.vertex_count; ++i) {
            const lab9::ProjectedPoint p = lab9::ProjectPoint(face.vertices[i], PView(1), PView(2));
            if (p.x < min_x) min_x = p.x;
            if (p.x > max_x) max_x = p.x;
            if (p.y < min_y) min_y = p.y;
            if (p.y > max_y) max_y = p.y;
        }
    }
    const int margin = 36;
    const int available_width = RW.Width() - 2 * margin;
    const int available_height = RW.Height() - 2 * margin;
    const double span_x = max_x - min_x;
    const double span_y = max_y - min_y;
    if (available_width <= 0 || available_height <= 0 || span_x <= 0.0 || span_y <= 0.0) {
        return;
    }
    double scale = static_cast<double>(available_width) / span_x;
    const double scale_y = static_cast<double>(available_height) / span_y;
    if (scale_y < scale) scale = scale_y;
    scale *= 0.9;
    const double center_x = (min_x + max_x) * 0.5;
    const double center_y = (min_y + max_y) * 0.5;
    const double pixel_center_x = (RW.left + RW.right) * 0.5;
    const double pixel_center_y = (RW.top + RW.bottom) * 0.5;

    for (const lab9::Face* face : visible) {
        std::vector<CPoint> points;
        points.reserve(face->vertex_count);
        for (std::size_t i = 0; i < face->vertex_count; ++i) {
            const lab9::ProjectedPoint p = lab9::ProjectPoint(face->vertices[i], PView(1), PView(2));
            const int px = static_cast<int>(std::lround(pixel_center_x + (p.x - center_x) * scale));
            const int py = static_cast<int>(std::lround(pixel_center_y - (p.y - center_y) * scale));
            points.emplace_back(px, py);
        }
        const lab9::ColorRGB shade = lab9::Shade(*face, light, source, 0.8);
        CPen pen(PS_SOLID, 2, RGB(20, 20, 20));
        CBrush brush(RGB(shade.r, shade.g, shade.b));
        CPen* old_pen = dc.SelectObject(&pen);
        CBrush* old_brush = dc.SelectObject(&brush);
        dc.Polygon(points.data(), static_cast<int>(points.size()));
        dc.SelectObject(old_pen);
        dc.SelectObject(old_brush);
    }
}
