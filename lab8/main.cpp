#include "CPlot3D.h"

#include <cwchar>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

namespace {

constexpr wchar_t window_class[] = L"Lab8SurfaceViewer";
constexpr UINT ID_SURFACE_ELLIPTIC = 41001;
constexpr UINT ID_SURFACE_HYPERBOLIC = 41002;
constexpr UINT ID_SURFACE_HEMISPHERE = 41003;
constexpr UINT ID_CAMERA_FROM_FILE = 41004;
constexpr UINT ID_CAMERA_DEFAULT = 41005;

struct AppState {
    lab8::CPlot3D plot;
    lab8::Camera camera;
    std::optional<lab8::SurfaceKind> surface;
};

HMENU CreateFigureMenu()
{
    HMENU menu = CreateMenu();
    if (!menu) return nullptr;
    HMENU figure = CreatePopupMenu();
    if (figure &&
        AppendMenuW(figure, MF_STRING, ID_CAMERA_FROM_FILE, L"Положение камеры") &&
        AppendMenuW(figure, MF_STRING, ID_SURFACE_ELLIPTIC, L"Эллиптический параболоид") &&
        AppendMenuW(figure, MF_STRING, ID_SURFACE_HYPERBOLIC, L"Гиперболический параболоид") &&
        AppendMenuW(figure, MF_STRING, ID_SURFACE_HEMISPHERE, L"Полусфера") &&
        AppendMenuW(figure, MF_STRING, ID_CAMERA_DEFAULT, L"Положение камеры по умолчанию") &&
        AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(figure), L"Фигура")) {
        return menu;
    }
    if (figure) DestroyMenu(figure);
    DestroyMenu(menu);
    return nullptr;
}

bool LoadCamera(lab8::Camera& camera)
{
    std::wstring executable(32768, L'\0');
    const DWORD capacity = static_cast<DWORD>(executable.size());
    const DWORD length = GetModuleFileNameW(nullptr, executable.data(), capacity);
    if (length == 0 || length >= capacity) return false;
    executable.resize(length);
    const auto path = std::filesystem::path(executable).parent_path() / L"angles.txt";
    std::ifstream input(path);
    return lab8::ReadCameraAngles(input, camera);
}

void Paint(HWND window, const AppState& state)
{
    PAINTSTRUCT paint{};
    HDC dc = BeginPaint(window, &paint);
    RECT client{};
    GetClientRect(window, &client);
    SetMapMode(dc, MM_TEXT);
    FillRect(dc, &client, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
    if (state.surface) {
        state.plot.Draw(dc, client, *state.surface, state.camera.azimuth, state.camera.zenith);
    }
    wchar_t label[96];
    const int length = std::swprintf(label, sizeof(label) / sizeof(*label),
                                     L"φ = %.12g°, θ = %.12g°", state.camera.azimuth, state.camera.zenith);
    if (length > 0) {
        SetBkMode(dc, TRANSPARENT);
        TextOutW(dc, 8, 8, label, length);
    }
    EndPaint(window, &paint);
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (message == WM_NCCREATE) {
        const auto* create = reinterpret_cast<const CREATESTRUCTW*>(lparam);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
    }
    auto* state = reinterpret_cast<AppState*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (!state) return DefWindowProcW(window, message, wparam, lparam);
    switch (message) {
    case WM_COMMAND:
        switch (LOWORD(wparam)) {
        case ID_SURFACE_ELLIPTIC: state->surface = lab8::SurfaceKind::EllipticParaboloid; break;
        case ID_SURFACE_HYPERBOLIC: state->surface = lab8::SurfaceKind::HyperbolicParaboloid; break;
        case ID_SURFACE_HEMISPHERE: state->surface = lab8::SurfaceKind::UpperHemisphere; break;
        case ID_CAMERA_FROM_FILE:
            if (!LoadCamera(state->camera)) {
                MessageBoxW(window, L"В angles.txt рядом с lab8.exe должны быть два числа: азимут и зенит от 0° до 180°.",
                            L"Положение камеры", MB_OK | MB_ICONWARNING);
                return 0;
            }
            break;
        case ID_CAMERA_DEFAULT: state->camera = {}; break;
        default: return DefWindowProcW(window, message, wparam, lparam);
        }
        InvalidateRect(window, nullptr, FALSE);
        return 0;
    case WM_PAINT:
        Paint(window, *state);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(window, message, wparam, lparam);
    }
}

}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show)
{
    WNDCLASSW wc{};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = window_class;
    if (!RegisterClassW(&wc)) return 1;

    HMENU menu = CreateFigureMenu();
    if (!menu) return 1;
    AppState state;
    HWND window = CreateWindowExW(0, window_class, L"Лабораторная работа 8 — поверхности",
                                  WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 900, 650,
                                  nullptr, menu, instance, &state);
    if (!window) {
        DestroyMenu(menu);
        return 1;
    }
    ShowWindow(window, show);
    UpdateWindow(window);

    MSG message{};
    BOOL result;
    while ((result = GetMessageW(&message, nullptr, 0, 0)) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return result == -1 ? 1 : static_cast<int>(message.wParam);
}
