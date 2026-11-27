#pragma once

#include <afxwin.h>

#include "CurveCore.h"

#include <vector>

struct PlotSeries {
    std::vector<lab11::Point> points;
    COLORREF color;
    int pen_style;
    int pen_width;
    bool connect_points;
    bool show_markers;
};

class CPlot2D {
public:
    void Draw(CDC& dc, const CRect& window_rect, const std::vector<PlotSeries>& series,
              const CString& title) const;
};
