#include "Plot2D.h"
#include <algorithm>
#include <cmath>

void CPlot2D::SetParams(CMatrix& xx, CMatrix& yy, CRect& rwx)
{
    X = xx; Y = yy; RW = rwx;
    if (X.Cols() == 0) return;

    double xmin = X(0, 0), xmax = X(0, 0), ymin = Y(0, 0), ymax = Y(0, 0);
    for (int i = 1; i < X.Cols(); ++i) {
        xmin = (std::min)(xmin, X(0, i)); xmax = (std::max)(xmax, X(0, i));
        ymin = (std::min)(ymin, Y(0, i)); ymax = (std::max)(ymax, Y(0, i));
    }
    if (xmax == xmin) { xmin -= 1; xmax += 1; }
    if (ymax == ymin) { ymin -= 1; ymax += 1; }

    RS = CRectD(xmin, ymax, xmax, ymin);
    K = SpaceToWindow(RS, RW);
}

void CPlot2D::SetWindowRect(CRect& rwx)
{
    RW = rwx;
    K = SpaceToWindow(RS, RW);
}

void CPlot2D::GetWindowCoords(double xs, double ys, int& xw, int& yw)
{
    xw = (int)std::lround(K(0, 0) * xs + K(0, 2));
    yw = (int)std::lround(K(1, 1) * ys + K(1, 2));
}

void CPlot2D::DrawAxesManual(CDC& dc, const double* xTicks, int xCount, const wchar_t** xLabels, const double* yTicks, int yCount)
{
    int x0, y0;
    GetWindowCoords(0, 0, x0, y0);

    dc.MoveTo(x0, RW.top); dc.LineTo(x0, RW.bottom);
    dc.MoveTo(RW.left, y0); dc.LineTo(RW.right, y0);

    dc.MoveTo(RW.right, y0); dc.LineTo(RW.right - 8, y0 - 4);
    dc.MoveTo(RW.right, y0); dc.LineTo(RW.right - 8, y0 + 4);
    dc.TextOut(RW.right - 16, y0 - 18, _T("X"));

    dc.MoveTo(x0, RW.top); dc.LineTo(x0 - 4, RW.top + 8);
    dc.MoveTo(x0, RW.top); dc.LineTo(x0 + 4, RW.top + 8);
    dc.TextOut(x0 + 6, RW.top + 2, _T("Y"));

    for (int i = 0; i < xCount; ++i) {
        int tick;
        GetWindowCoords(xTicks[i], 0, tick, y0);
        dc.MoveTo(tick, y0 - 4); dc.LineTo(tick, y0 + 4);
        dc.TextOut(tick - 10, y0 + 6, xLabels[i]);
    }

    CString s;
    for (int i = 0; i < yCount; ++i) {
        int tick;
        GetWindowCoords(0, yTicks[i], x0, tick);
        dc.MoveTo(x0 - 4, tick); dc.LineTo(x0 + 4, tick);
        if (std::fabs(yTicks[i] - std::round(yTicks[i])) < 1e-9) s.Format(_T("%.0f"), yTicks[i]);
        else s.Format(_T("%.1f"), yTicks[i]);
        dc.TextOut(x0 - 34, tick - 7, s);
    }

    dc.TextOut(x0 + 4, y0 + 4, _T("0"));
}

void CPlot2D::Draw(CDC& dc, int ind1, int ind2)
{
    CPen line(PenLine.PenStyle, PenLine.PenWidth, PenLine.PenColor);
    CPen* old = dc.SelectObject(&line);

    int x, y;
    GetWindowCoords(X(0, ind1), Y(0, ind1), x, y);
    dc.MoveTo(x, y);
    for (int i = ind1 + 1; i <= ind2; ++i) {
        GetWindowCoords(X(0, i), Y(0, i), x, y);
        dc.LineTo(x, y);
    }
    dc.SelectObject(old);
}

void CPlot2D::DrawCircleAndOctagon(CDC& dc, CRect& area)
{
    const double R = 5.0;
    const double PI = 3.141592653589793;

    CRectD world(-R - 1, R + 1, R + 1, -R - 1);
    CMatrix k = SpaceToWindow(world, area);
    auto point = [&](double x, double y, int& px, int& py) {
        px = (int)std::lround(k(0, 0) * x + k(0, 2));
        py = (int)std::lround(k(1, 1) * y + k(1, 2));
        };

    CPen* old = dc.GetCurrentPen();

    CPen circle(PS_SOLID, 2, RGB(0, 0, 255));
    dc.SelectObject(&circle);
    int left, top, right, bottom;
    point(-R, R, left, top);
    point(R, -R, right, bottom);
    dc.Ellipse(left, top, right, bottom);

    CPen polygon(PS_SOLID, 3, RGB(255, 0, 0));
    dc.SelectObject(&polygon);
    const double phi = PI / 2.0;
    const double angles = 10.0;
    for (int i = 0; i < angles; ++i) {
        int x1, y1, x2, y2;
        point(R * cos(2 * PI * i / angles + phi),       R * sin(2 * PI * i / angles + phi),       x1, y1);
        point(R * cos(2 * PI * (i + 1) / angles + phi), R * sin(2 * PI * (i + 1) / angles + phi), x2, y2);
        dc.MoveTo(x1, y1);
        dc.LineTo(x2, y2);
    }

    int x0, y0, tick;
    CString s;
    point(0, 0, x0, y0);

    CPen axisPen(PS_SOLID, 1, RGB(0, 0, 0));
    dc.SelectObject(&axisPen);
    dc.MoveTo(x0, area.top); dc.LineTo(x0, area.bottom);
    dc.MoveTo(area.left, y0); dc.LineTo(area.right, y0);

    dc.MoveTo(area.right, y0); dc.LineTo(area.right - 8, y0 - 4);
    dc.MoveTo(area.right, y0); dc.LineTo(area.right - 8, y0 + 4);
    dc.TextOut(area.right - 16, y0 - 18, _T("X"));

    dc.MoveTo(x0, area.top); dc.LineTo(x0 - 4, area.top + 8);
    dc.MoveTo(x0, area.top); dc.LineTo(x0 + 4, area.top + 8);
    dc.TextOut(x0 + 6, area.top + 2, _T("Y"));

    for (int i = -(int)R; i <= (int)R; i += 5) {   // подписи осей привязаны к радиусу
        if (i == 0) continue;
        s.Format(_T("%d"), i);

        point(i, 0, tick, y0);
        dc.MoveTo(tick, y0 - 4);
        dc.LineTo(tick, y0 + 4);
        dc.TextOut(tick - 8, y0 + 6, s);

        point(0, i, x0, tick);
        dc.MoveTo(x0 - 4, tick);
        dc.LineTo(x0 + 4, tick);
        dc.TextOut(x0 - 24, tick - 7, s);
    }

    dc.TextOut(x0 + 4, y0 + 4, _T("0"));
    dc.SelectObject(old);
}