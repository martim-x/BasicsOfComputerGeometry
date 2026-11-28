#include <afxwin.h>
#include <afxdlgs.h>
#include <cmath>
#include <fstream>

#include "SphereRenderer.h"

namespace {

constexpr UINT ID_SPHERE_DIFFUSE = 43001;
constexpr UINT ID_SPHERE_SPECULAR = 43002;
constexpr UINT ID_CAMERA_FROM_FILE = 43003;
constexpr UINT ID_CAMERA_DEFAULT = 43004;
constexpr UINT ID_LIGHT_FROM_FILE = 43005;
constexpr UINT ID_LIGHT_DEFAULT = 43006;
constexpr UINT ID_COLOR_PICKER = 43007;
constexpr double sphere_radius = 2.0;

bool read_spherical(const char* path, double min_radius, double& radius, double& azimuth, double& zenith)
{
    std::ifstream input(path);
    if (!(input >> radius >> azimuth >> zenith) || !std::isfinite(radius) ||
        !std::isfinite(azimuth) || !std::isfinite(zenith) || radius <= min_radius ||
        zenith < 0.0 || zenith > 180.0 || !(input >> std::ws).eof()) {
        return false;
    }
    azimuth = std::fmod(azimuth, 360.0);
    if (azimuth < 0.0) azimuth += 360.0;
    if (azimuth >= 360.0 || azimuth == 0.0) azimuth = 0.0;
    return true;
}

}

class CMainWin final : public CFrameWnd {
public:
    CMainWin() : m_camera(15.0, 30.0, 60.0), m_light(10.0, 45.0, 45.0)
    {
        Create(nullptr, _T("Лабораторная работа 10 — освещение шара"));
    }

protected:
    afx_msg int OnCreate(LPCREATESTRUCT create_struct)
    {
        if (CFrameWnd::OnCreate(create_struct) == -1) {
            return -1;
        }
        m_menu.CreateMenu();
        m_figure_menu.CreatePopupMenu();
        m_figure_menu.AppendMenu(MF_STRING, ID_CAMERA_FROM_FILE, _T("Камера (r, φ, θ)"));
        m_figure_menu.AppendMenu(MF_STRING, ID_LIGHT_FROM_FILE, _T("Источник света (r, φ, θ)"));
        m_figure_menu.AppendMenu(MF_STRING, ID_COLOR_PICKER, _T("Цвет источника света (R, G, B)"));
        m_sphere_menu.CreatePopupMenu();
        m_sphere_menu.AppendMenu(MF_STRING, ID_SPHERE_DIFFUSE, _T("Диффузная модель"));
        m_sphere_menu.AppendMenu(MF_STRING, ID_SPHERE_SPECULAR, _T("Зеркальная модель"));
        m_figure_menu.AppendMenu(MF_POPUP, reinterpret_cast<UINT_PTR>(m_sphere_menu.Detach()), _T("Шар"));
        m_figure_menu.AppendMenu(MF_SEPARATOR);
        m_figure_menu.AppendMenu(MF_STRING, ID_CAMERA_DEFAULT, _T("Камера по умолчанию"));
        m_figure_menu.AppendMenu(MF_STRING, ID_LIGHT_DEFAULT, _T("Источник света по умолчанию"));
        m_menu.AppendMenu(MF_POPUP, reinterpret_cast<UINT_PTR>(m_figure_menu.Detach()), _T("Фигура"));
        SetMenu(&m_menu);
        return 0;
    }

    afx_msg void OnPaint()
    {
        CPaintDC dc(this);
        CRect client;
        GetClientRect(&client);
        dc.FillSolidRect(client, RGB(255, 255, 255));
        if (m_mode < 0) {
            return;
        }
        DrawLightSphere(dc, sphere_radius, m_camera, m_light, client, m_light_color, m_mode);
        dc.SetBkMode(TRANSPARENT);
        CString camera_text;
        CString light_text;
        camera_text.Format(_T("Камера: r=%.0f, φ=%.0f°, θ=%.0f°"), m_camera(0), m_camera(1), m_camera(2));
        light_text.Format(_T("Свет: r=%.0f, φ=%.0f°, θ=%.0f°   RGB=(%u,%u,%u)"),
                          m_light(0), m_light(1), m_light(2),
                          static_cast<unsigned>(GetRValue(m_light_color)),
                          static_cast<unsigned>(GetGValue(m_light_color)),
                          static_cast<unsigned>(GetBValue(m_light_color)));
        dc.TextOut(8, 8, camera_text);
        dc.TextOut(8, 28, light_text);
    }

    afx_msg void OnSize(UINT type, int cx, int cy)
    {
        CFrameWnd::OnSize(type, cx, cy);
        if (cx > 0 && cy > 0) {
            Invalidate(FALSE);
        }
    }

    afx_msg void OnDiffuse()
    {
        m_mode = 0;
        Invalidate(FALSE);
    }

    afx_msg void OnSpecular()
    {
        m_mode = 1;
        Invalidate(FALSE);
    }

    afx_msg void OnCameraFromFile()
    {
        double radius = 0.0;
        double azimuth = 0.0;
        double zenith = 0.0;
        if (!read_spherical("camera.txt", sphere_radius, radius, azimuth, zenith)) {
            AfxMessageBox(_T("camera.txt должен содержать r, азимут и зенит; r > 2, 0° ≤ зенит ≤ 180°."), MB_ICONWARNING);
            return;
        }
        m_camera = CMatrix(radius, azimuth, zenith);
        Invalidate(FALSE);
    }

    afx_msg void OnCameraDefault()
    {
        m_camera = CMatrix(15.0, 30.0, 60.0);
        Invalidate(FALSE);
    }

    afx_msg void OnLightFromFile()
    {
        double radius = 0.0;
        double azimuth = 0.0;
        double zenith = 0.0;
        if (!read_spherical("light.txt", 0.0, radius, azimuth, zenith)) {
            AfxMessageBox(_T("light.txt должен содержать r, азимут и зенит; r > 0, 0° ≤ зенит ≤ 180°."), MB_ICONWARNING);
            return;
        }
        m_light = CMatrix(radius, azimuth, zenith);
        Invalidate(FALSE);
    }

    afx_msg void OnLightDefault()
    {
        m_light = CMatrix(10.0, 45.0, 45.0);
        Invalidate(FALSE);
    }

    afx_msg void OnColorPicker()
    {
        CColorDialog dialog(m_light_color, CC_FULLOPEN);
        if (dialog.DoModal() == IDOK) {
            m_light_color = dialog.GetColor();
            Invalidate(FALSE);
        }
    }

    DECLARE_MESSAGE_MAP()

private:
    CMenu m_menu;
    CMenu m_figure_menu;
    CMenu m_sphere_menu;
    CMatrix m_camera;
    CMatrix m_light;
    COLORREF m_light_color = RGB(255, 255, 0);
    int m_mode = -1;
};

BEGIN_MESSAGE_MAP(CMainWin, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_COMMAND(ID_SPHERE_DIFFUSE, &CMainWin::OnDiffuse)
    ON_COMMAND(ID_SPHERE_SPECULAR, &CMainWin::OnSpecular)
    ON_COMMAND(ID_CAMERA_FROM_FILE, &CMainWin::OnCameraFromFile)
    ON_COMMAND(ID_CAMERA_DEFAULT, &CMainWin::OnCameraDefault)
    ON_COMMAND(ID_LIGHT_FROM_FILE, &CMainWin::OnLightFromFile)
    ON_COMMAND(ID_LIGHT_DEFAULT, &CMainWin::OnLightDefault)
    ON_COMMAND(ID_COLOR_PICKER, &CMainWin::OnColorPicker)
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
