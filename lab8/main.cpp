#include <afxwin.h>
#include <fstream>

#include "CPlot3D.h"

namespace {

constexpr UINT ID_SURFACE_ELLIPTIC = 41001;
constexpr UINT ID_SURFACE_HYPERBOLIC = 41002;
constexpr UINT ID_SURFACE_HEMISPHERE = 41003;
constexpr UINT ID_CAMERA_FROM_FILE = 41004;
constexpr UINT ID_CAMERA_DEFAULT = 41005;

}

class CMainWin final : public CFrameWnd {
public:
    CMainWin()
    {
        Create(nullptr, _T("Лабораторная работа 8 — поверхности"));
    }

protected:
    afx_msg int OnCreate(LPCREATESTRUCT create_struct)
    {
        if (CFrameWnd::OnCreate(create_struct) == -1) {
            return -1;
        }
        m_menu.CreateMenu();
        m_surface_menu.CreatePopupMenu();
        m_surface_menu.AppendMenu(MF_STRING, ID_SURFACE_ELLIPTIC, _T("Эллиптический параболоид"));
        m_surface_menu.AppendMenu(MF_STRING, ID_SURFACE_HYPERBOLIC, _T("Гиперболический параболоид"));
        m_surface_menu.AppendMenu(MF_STRING, ID_SURFACE_HEMISPHERE, _T("Верхняя полусфера"));
        m_menu.AppendMenu(MF_POPUP, reinterpret_cast<UINT_PTR>(m_surface_menu.Detach()), _T("Фигура"));

        m_camera_menu.CreatePopupMenu();
        m_camera_menu.AppendMenu(MF_STRING, ID_CAMERA_FROM_FILE, _T("Текущие углы из angles.txt"));
        m_camera_menu.AppendMenu(MF_STRING, ID_CAMERA_DEFAULT, _T("По умолчанию"));
        m_menu.AppendMenu(MF_POPUP, reinterpret_cast<UINT_PTR>(m_camera_menu.Detach()), _T("Камера"));
        SetMenu(&m_menu);
        return 0;
    }

    afx_msg void OnPaint()
    {
        CPaintDC dc(this);
        CRect client;
        GetClientRect(&client);
        dc.FillSolidRect(client, RGB(255, 255, 255));
        if (m_has_surface) {
            m_plot.Draw(dc, client, m_surface, m_azimuth, m_zenith);
            CString label;
            label.Format(_T("φ = %.0f°, θ = %.0f°"), m_azimuth, m_zenith);
            dc.SetBkMode(TRANSPARENT);
            dc.TextOut(8, 8, label);
        }
    }

    afx_msg void OnSize(UINT type, int cx, int cy)
    {
        CFrameWnd::OnSize(type, cx, cy);
        if (cx > 0 && cy > 0) {
            Invalidate(FALSE);
        }
    }

    afx_msg void OnElliptic()
    {
        m_surface = lab8::SurfaceKind::EllipticParaboloid;
        m_has_surface = true;
        Invalidate(FALSE);
    }

    afx_msg void OnHyperbolic()
    {
        m_surface = lab8::SurfaceKind::HyperbolicParaboloid;
        m_has_surface = true;
        Invalidate(FALSE);
    }

    afx_msg void OnHemisphere()
    {
        m_surface = lab8::SurfaceKind::UpperHemisphere;
        m_has_surface = true;
        Invalidate(FALSE);
    }

    afx_msg void OnCameraFromFile()
    {
        std::ifstream input("angles.txt");
        double azimuth = 0.0;
        double zenith = 0.0;
        if (!(input >> azimuth >> zenith) || zenith < 0.0 || zenith > 180.0) {
            AfxMessageBox(_T("Не удалось прочитать angles.txt. Нужны два угла: азимут и зенит."), MB_ICONWARNING);
            return;
        }
        m_azimuth = azimuth;
        while (m_azimuth < 0.0) m_azimuth += 360.0;
        while (m_azimuth >= 360.0) m_azimuth -= 360.0;
        m_zenith = zenith;
        Invalidate(FALSE);
    }

    afx_msg void OnCameraDefault()
    {
        m_azimuth = 45.0;
        m_zenith = 45.0;
        Invalidate(FALSE);
    }

    DECLARE_MESSAGE_MAP()

private:
    CMenu m_menu;
    CMenu m_surface_menu;
    CMenu m_camera_menu;
    lab8::CPlot3D m_plot;
    lab8::SurfaceKind m_surface = lab8::SurfaceKind::EllipticParaboloid;
    double m_azimuth = 45.0;
    double m_zenith = 45.0;
    bool m_has_surface = false;
};

BEGIN_MESSAGE_MAP(CMainWin, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_COMMAND(ID_SURFACE_ELLIPTIC, &CMainWin::OnElliptic)
    ON_COMMAND(ID_SURFACE_HYPERBOLIC, &CMainWin::OnHyperbolic)
    ON_COMMAND(ID_SURFACE_HEMISPHERE, &CMainWin::OnHemisphere)
    ON_COMMAND(ID_CAMERA_FROM_FILE, &CMainWin::OnCameraFromFile)
    ON_COMMAND(ID_CAMERA_DEFAULT, &CMainWin::OnCameraDefault)
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
