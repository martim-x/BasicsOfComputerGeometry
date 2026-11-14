#include "ChildView.h"
#include <algorithm>
#include <cmath>

BEGIN_MESSAGE_MAP(CChildView, CWnd)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_COMMAND(ID_TEST_F1, &CChildView::OnTestF1)
    ON_COMMAND(ID_TEST_F2, &CChildView::OnTestF2)
    ON_COMMAND(ID_TEST_F3, &CChildView::OnTestF3)
END_MESSAGE_MAP()

int CChildView::OnCreate(LPCREATESTRUCT create)
{
    if (CWnd::OnCreate(create) == -1) return -1;
    BuildFunction(false);
    return 0;
}

double CChildView::MyF1(double x) { return std::sin(x)/x; }
double CChildView::MyF2(double x) { return std::sqrt(std::fabs(x)) * std::sin(x); }

void CChildView::BuildFunction(bool second)
{
    const double PI = 3.141592653589793;
    const double x0 = second ? -4.0 * PI : -3.0 * PI;
    const double x1 = second ? 4.0 * PI : 3.0 * PI;
    const double dx = PI / 36.0;
    const int count = (int)std::lround((x1 - x0) / dx) + 1;
    xValues.RedimMatrix(1, count);
    yValues.RedimMatrix(1, count);
    for (int i = 0; i < count; ++i) {
        const double x = x0 + i * dx;
        xValues(0, i) = x;
        yValues(0, i) = second ? MyF2(x) : MyF1(x);
    }
}

void CChildView::DrawFunction(CDC& dc)
{
    CRect client;
    GetClientRect(&client);
    CRect area(client.Width() / 10, client.Height() / 10,
        client.Width() * 9 / 10, client.Height() * 9 / 10);

    CMyPen line, axis;
    if (testMode == 1) {
        line.Set(PS_SOLID, 1, RGB(255, 0, 0));
        axis.Set(PS_SOLID, 2, RGB(0, 0, 255));
    }
    else {
        line.Set(PS_DASHDOT, 1, RGB(255, 0, 0));
        axis.Set(PS_SOLID, 2, RGB(0, 0, 0));
    }

    plot.SetParams(xValues, yValues, area);
    plot.SetPenLine(line);
    plot.SetPenAxis(axis);

    CPen axisPen(axis.PenStyle, axis.PenWidth, axis.PenColor);
    CPen* oldPen = dc.SelectObject(&axisPen);
    dc.Rectangle(area);

    const double PI = 3.141592653589793;
    if (testMode == 1) {
        double xTicks[] = { -3 * PI, -2 * PI, -PI, 0, PI, 2 * PI, 3 * PI };
        const wchar_t* xLabels[] = { L"-3PI", L"-2PI", L"-PI", L"0", L"PI", L"2PI", L"3PI" };
        double yTicks[] = { -0.2, 0, 0.2, 0.4, 0.6, 0.8, 1.0 };
        plot.DrawAxesManual(dc, xTicks, 7, xLabels, yTicks, 7);
    }
    else if (testMode == 2) {
        double xTicks[] = { -4 * PI, -2 * PI, 2 * PI, 4 * PI };
        const wchar_t* xLabels[] = { L"-4PI", L"-2PI", L"2PI", L"4PI" };
        double yTicks[] = { -2, -1, 1, 2 };
        plot.DrawAxesManual(dc, xTicks, 4, xLabels, yTicks, 4);
    }

    dc.SelectObject(oldPen);

    plot.Draw(dc, 0, xValues.Cols() - 1);
}

void CChildView::OnPaint()
{
    CPaintDC dc(this);
    CRect client;
    GetClientRect(&client);
    dc.FillSolidRect(client, RGB(255, 255, 255));
    if (testMode == 3) {
        const double scale = 2.0;   // допустимые значения: 2.0 или 4.0
        const double maxScale = 4.0;
        const double fraction = 0.8 * scale / maxScale;
        const int side = (int)((std::min)(client.Width(), client.Height()) * fraction);
        CRect area((client.Width() - side) / 2, (client.Height() - side) / 2,
                   (client.Width() + side) / 2, (client.Height() + side) / 2);
        CPlot2D shape;
        shape.DrawCircleAndOctagon(dc, area);
    } else {
        DrawFunction(dc);
    }
}

void CChildView::OnSize(UINT type, int cx, int cy)
{
    CWnd::OnSize(type, cx, cy);
    Invalidate(FALSE);
}

void CChildView::OnTestF1() { testMode = 1; BuildFunction(false); Invalidate(); }
void CChildView::OnTestF2() { testMode = 2; BuildFunction(true); Invalidate(); }
void CChildView::OnTestF3() { testMode = 3; Invalidate(); }