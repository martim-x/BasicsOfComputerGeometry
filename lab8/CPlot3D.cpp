#include "CPlot3D.h"

#include <cmath>
#include <vector>

namespace lab8 {
namespace {

struct ScreenTriangle {
    CPoint points[3];
};

void include_point(const ProjectedPoint& point, double& min_x, double& max_x,
                   double& min_y, double& max_y, bool& initialized)
{
    if (!initialized) {
        min_x = max_x = point.x;
        min_y = max_y = point.y;
        initialized = true;
        return;
    }
    if (point.x < min_x) min_x = point.x;
    if (point.x > max_x) max_x = point.x;
    if (point.y < min_y) min_y = point.y;
    if (point.y > max_y) max_y = point.y;
}

}

void CPlot3D::Draw(CDC& dc, const CRect& window_rect, SurfaceKind kind,
                   double azimuth_degrees, double zenith_degrees) const
{
    double x_min = -5.0;
    double x_max = 5.0;
    double y_min = -5.0;
    double y_max = 5.0;
    COLORREF color = RGB(105, 168, 235);
    if (kind == SurfaceKind::UpperHemisphere) {
        x_min = -3.0;
        x_max = 3.0;
        y_min = -3.0;
        y_max = 3.0;
        color = RGB(104, 190, 130);
    } else if (kind == SurfaceKind::HyperbolicParaboloid) {
        color = RGB(222, 148, 95);
    }

    std::vector<Triangle> triangles = BuildSurface(kind, x_min, x_max, y_min, y_max, 0.25);
    SortFarToNear(triangles, CameraDirection(azimuth_degrees, zenith_degrees));
    if (triangles.empty()) {
        return;
    }

    std::vector<ScreenTriangle> screen_triangles;
    screen_triangles.reserve(triangles.size());
    double min_x = 0.0;
    double max_x = 0.0;
    double min_y = 0.0;
    double max_y = 0.0;
    bool initialized = false;
    for (const Triangle& triangle : triangles) {
        const ProjectedPoint projected[3] = {
            ProjectPoint(triangle.p0, azimuth_degrees, zenith_degrees),
            ProjectPoint(triangle.p1, azimuth_degrees, zenith_degrees),
            ProjectPoint(triangle.p2, azimuth_degrees, zenith_degrees)
        };
        ScreenTriangle screen{};
        for (int i = 0; i < 3; ++i) {
            include_point(projected[i], min_x, max_x, min_y, max_y, initialized);
        }
        screen_triangles.push_back(screen);
    }

    const int margin = 36;
    const int available_width = window_rect.Width() - 2 * margin;
    const int available_height = window_rect.Height() - 2 * margin;
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
    const double pixel_center_x = (window_rect.left + window_rect.right) * 0.5;
    const double pixel_center_y = (window_rect.top + window_rect.bottom) * 0.5;

    for (std::size_t n = 0; n < triangles.size(); ++n) {
        const Vec3* vertices[3] = {&triangles[n].p0, &triangles[n].p1, &triangles[n].p2};
        for (int i = 0; i < 3; ++i) {
            const ProjectedPoint p = ProjectPoint(*vertices[i], azimuth_degrees, zenith_degrees);
            const int px = static_cast<int>(std::lround(pixel_center_x + (p.x - center_x) * scale));
            const int py = static_cast<int>(std::lround(pixel_center_y - (p.y - center_y) * scale));
            screen_triangles[n].points[i] = CPoint(px, py);
        }
    }

    CPen pen(PS_SOLID, 1, RGB(30, 45, 60));
    CBrush brush(color);
    CPen* old_pen = dc.SelectObject(&pen);
    CBrush* old_brush = dc.SelectObject(&brush);
    for (const ScreenTriangle& triangle : screen_triangles) {
        dc.Polygon(triangle.points, 3);
    }
    dc.SelectObject(old_pen);
    dc.SelectObject(old_brush);
}

}
