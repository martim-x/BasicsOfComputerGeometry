#include "CPlot2D.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace {

struct Bounds {
    double min_x;
    double max_x;
    double min_y;
    double max_y;
};

CPoint to_window(const lab11::Point& point, const Bounds& bounds, const CRect& plot)
{
    const double x_ratio = (point.x - bounds.min_x) / (bounds.max_x - bounds.min_x);
    const double y_ratio = (point.y - bounds.min_y) / (bounds.max_y - bounds.min_y);
    return CPoint(plot.left + static_cast<int>(std::lround(x_ratio * plot.Width())),
                  plot.bottom - static_cast<int>(std::lround(y_ratio * plot.Height())));
}

}

void CPlot2D::Draw(CDC& dc, const CRect& window_rect, const std::vector<PlotSeries>& series,
                   const CString& title) const
{
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(RGB(35, 40, 50));
    dc.TextOut(window_rect.left + 18, window_rect.top + 14, title);
    if (window_rect.Width() < 180 || window_rect.Height() < 160) {
        return;
    }

    Bounds bounds{std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(),
                  std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()};
    bool has_points = false;
    for (const PlotSeries& item : series) {
        for (const lab11::Point& point : item.points) {
            if (!std::isfinite(point.x) || !std::isfinite(point.y)) {
                continue;
            }
            has_points = true;
            bounds.min_x = (std::min)(bounds.min_x, point.x);
            bounds.max_x = (std::max)(bounds.max_x, point.x);
            bounds.min_y = (std::min)(bounds.min_y, point.y);
            bounds.max_y = (std::max)(bounds.max_y, point.y);
        }
    }
    if (!has_points) {
        dc.TextOut(window_rect.left + 24, window_rect.top + 54, _T("Выбери Lines → Lagr или Lines → Bezier."));
        return;
    }

    if (bounds.min_x == bounds.max_x) {
        bounds.min_x -= 1.0;
        bounds.max_x += 1.0;
    }
    if (bounds.min_y == bounds.max_y) {
        bounds.min_y -= 1.0;
        bounds.max_y += 1.0;
    }
    const double pad_x = (bounds.max_x - bounds.min_x) * 0.06;
    const double pad_y = (bounds.max_y - bounds.min_y) * 0.08;
    bounds.min_x -= pad_x;
    bounds.max_x += pad_x;
    bounds.min_y -= pad_y;
    bounds.max_y += pad_y;

    CRect plot(window_rect.left + 76, window_rect.top + 48,
               window_rect.right - 30, window_rect.bottom - 58);
    if (plot.Width() <= 0 || plot.Height() <= 0) {
        return;
    }

    CPen grid_pen(PS_DOT, 1, RGB(220, 224, 230));
    CPen* old_pen = dc.SelectObject(&grid_pen);
    for (int tick = 0; tick <= 4; ++tick) {
        const int x = plot.left + plot.Width() * tick / 4;
        const int y = plot.top + plot.Height() * tick / 4;
        dc.MoveTo(x, plot.top);
        dc.LineTo(x, plot.bottom);
        dc.MoveTo(plot.left, y);
        dc.LineTo(plot.right, y);
    }
    dc.SelectObject(old_pen);

    CPen axis_pen(PS_SOLID, 1, RGB(75, 82, 92));
    old_pen = dc.SelectObject(&axis_pen);
    const int x_axis = bounds.min_y <= 0.0 && bounds.max_y >= 0.0
        ? to_window({bounds.min_x, 0.0}, bounds, plot).y : plot.bottom;
    const int y_axis = bounds.min_x <= 0.0 && bounds.max_x >= 0.0
        ? to_window({0.0, bounds.min_y}, bounds, plot).x : plot.left;
    dc.MoveTo(plot.left, x_axis);
    dc.LineTo(plot.right, x_axis);
    dc.MoveTo(y_axis, plot.top);
    dc.LineTo(y_axis, plot.bottom);
    dc.Rectangle(plot);
    dc.SelectObject(old_pen);

    for (int tick = 0; tick <= 4; ++tick) {
        CString label;
        const double x_value = bounds.min_x + (bounds.max_x - bounds.min_x) * tick / 4.0;
        const double y_value = bounds.max_y - (bounds.max_y - bounds.min_y) * tick / 4.0;
        label.Format(_T("%.3g"), x_value);
        dc.TextOut(plot.left + plot.Width() * tick / 4 - 16, plot.bottom + 6, label);
        label.Format(_T("%.3g"), y_value);
        dc.TextOut(window_rect.left + 8, plot.top + plot.Height() * tick / 4 - 8, label);
    }
    dc.TextOut(plot.right - 10, window_rect.bottom - 25, _T("X"));
    dc.TextOut(window_rect.left + 42, plot.top - 18, _T("Y"));

    for (const PlotSeries& item : series) {
        if (item.points.empty()) {
            continue;
        }
        CPen pen(item.pen_style, item.pen_width, item.color);
        old_pen = dc.SelectObject(&pen);
        bool started = false;
        for (const lab11::Point& point : item.points) {
            if (!std::isfinite(point.x) || !std::isfinite(point.y)) {
                started = false;
                continue;
            }
            const CPoint location = to_window(point, bounds, plot);
            if (item.connect_points) {
                if (!started) {
                    dc.MoveTo(location);
                    started = true;
                } else {
                    dc.LineTo(location);
                }
            }
            if (item.show_markers) {
                CBrush brush(item.color);
                CBrush* old_brush = dc.SelectObject(&brush);
                dc.Ellipse(location.x - 3, location.y - 3, location.x + 4, location.y + 4);
                dc.SelectObject(old_brush);
            }
        }
        dc.SelectObject(old_pen);
    }
}
