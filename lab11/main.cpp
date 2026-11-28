#include <afxwin.h>

#include "CPlot2D.h"
#include "CurveCore.h"

#include <exception>
#include <string>
#include <vector>

namespace {

constexpr UINT ID_LINES_LAGR = 44001;
constexpr UINT ID_LINES_BEZIER = 44002;
constexpr int bezier_segments = 100;

CMatrix make_matrix(const std::vector<lab11::Point>& points, bool use_x)
{
    CMatrix values(static_cast<int>(points.size()));
    for (int i = 0; i < values.rows(); ++i) {
        values(i) = use_x ? points[static_cast<std::size_t>(i)].x
                          : points[static_cast<std::size_t>(i)].y;
    }
    return values;
}

std::vector<lab11::Point> sample_bezier(const std::vector<lab11::Point>& controls)
{
    CMatrix x = make_matrix(controls, true);
    CMatrix y = make_matrix(controls, false);
    CMatrix xb;
    CMatrix yb;
    Bezier(x, y, xb, yb, bezier_segments);
    std::vector<lab11::Point> curve;
    curve.reserve(static_cast<std::size_t>(xb.rows()));
    for (int i = 0; i < xb.rows(); ++i) {
        curve.push_back({xb(i), yb(i)});
    }
    return curve;
}

bool export_points(const std::string& path, const std::vector<lab11::Point>& points)
{
    try {
        lab11::WritePointFile(path, points);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

}

class CMainWin final : public CFrameWnd {
public:
    CMainWin()
    {
        Create(nullptr, _T("Лабораторная работа 11 — кривые на плоскости"),
               WS_OVERLAPPEDWINDOW, CRect(100, 100, 1040, 760));
    }

protected:
    afx_msg int OnCreate(LPCREATESTRUCT create_struct)
    {
        if (CFrameWnd::OnCreate(create_struct) == -1) {
            return -1;
        }
        m_main_menu.CreateMenu();
        m_lines_menu.CreatePopupMenu();
        m_lines_menu.AppendMenu(MF_STRING, ID_LINES_LAGR, _T("Lagr"));
        m_lines_menu.AppendMenu(MF_STRING, ID_LINES_BEZIER, _T("Bezier"));
        m_main_menu.AppendMenu(MF_POPUP, reinterpret_cast<UINT_PTR>(m_lines_menu.Detach()), _T("Lines"));
        SetMenu(&m_main_menu);
        return 0;
    }

    afx_msg void OnPaint()
    {
        CPaintDC dc(this);
        CRect client;
        GetClientRect(&client);
        dc.FillSolidRect(client, RGB(255, 255, 255));
        m_plot.Draw(dc, client, m_series, m_title);
    }

    afx_msg void OnSize(UINT type, int cx, int cy)
    {
        CFrameWnd::OnSize(type, cx, cy);
        if (cx > 0 && cy > 0) {
            Invalidate(FALSE);
        }
    }

    afx_msg void OnLagrange()
    {
        try {
            const std::vector<lab11::Point> nodes = lab11::AssignmentLagrangeNodes();
            const std::vector<lab11::Point> curve = lab11::AssignmentLagrangeCurve(nodes);

            const bool nodes_saved = export_points("Mathcad/lagrange_data.txt", nodes);
            const bool curve_saved = export_points("Mathcad/lagrange_curve.txt", curve);
            m_series.clear();
            m_series.push_back({nodes, RGB(210, 55, 55), PS_SOLID, 1, false, true});
            m_series.push_back({curve, RGB(35, 92, 190), PS_SOLID, 2, true, false});
            m_title = _T("Lagrange: red points (x_i,y_i), blue interpolant L(x)");
            Invalidate(FALSE);
            if (!nodes_saved || !curve_saved) {
                AfxMessageBox(_T("График построен, но не удалось сохранить данные в папку Mathcad."), MB_ICONWARNING);
            }
        } catch (const std::exception& error) {
            CString message;
            message.Format(_T("Не удалось построить интерполяцию. Ошибка: %S"), error.what());
            AfxMessageBox(message, MB_ICONERROR);
        }
    }

    afx_msg void OnBezier()
    {
        const auto quadratic_controls = lab11::AssignmentQuadraticControls();
        const auto cubic_controls = lab11::AssignmentCubicControls();
        const std::vector<lab11::Point> quadratic_curve = sample_bezier(quadratic_controls);
        const std::vector<lab11::Point> cubic_curve = sample_bezier(cubic_controls);
        const bool quadratic_control_saved = export_points("Mathcad/bezier_2_control.txt", quadratic_controls);
        const bool quadratic_curve_saved = export_points("Mathcad/bezier_2_curve.txt", quadratic_curve);
        const bool cubic_control_saved = export_points("Mathcad/bezier_3_control.txt", cubic_controls);
        const bool cubic_curve_saved = export_points("Mathcad/bezier_3_curve.txt", cubic_curve);

        m_series.clear();
        m_series.push_back({quadratic_controls, RGB(115, 120, 130), PS_DASH, 1, true, true});
        m_series.push_back({quadratic_curve, RGB(28, 112, 190), PS_SOLID, 2, true, false});
        m_series.push_back({cubic_controls, RGB(125, 110, 130), PS_DASH, 1, true, true});
        m_series.push_back({cubic_curve, RGB(205, 65, 55), PS_SOLID, 2, true, false});
        m_title.Format(_T("Bezier: dashed controls; blue degree 2; red degree 3; M=%d"), bezier_segments);
        Invalidate(FALSE);

        if (!quadratic_control_saved || !quadratic_curve_saved ||
            !cubic_control_saved || !cubic_curve_saved) {
            AfxMessageBox(_T("Кривые построены, но не удалось сохранить все наборы в папку Mathcad."), MB_ICONWARNING);
        }
    }

    DECLARE_MESSAGE_MAP()

private:
    CMenu m_main_menu;
    CMenu m_lines_menu;
    CPlot2D m_plot;
    std::vector<PlotSeries> m_series;
    CString m_title = _T("Выбери Lines → Lagr или Lines → Bezier.");
};

BEGIN_MESSAGE_MAP(CMainWin, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_COMMAND(ID_LINES_LAGR, &CMainWin::OnLagrange)
    ON_COMMAND(ID_LINES_BEZIER, &CMainWin::OnBezier)
END_MESSAGE_MAP()

class CGeometryApp final : public CWinApp {
public:
    BOOL InitInstance() override
    {
        CWinApp::InitInstance();
        auto* window = new CMainWin();
        m_pMainWnd = window;
        window->ShowWindow(SW_SHOW);
        window->UpdateWindow();
        return TRUE;
    }
};

CGeometryApp the_app;
