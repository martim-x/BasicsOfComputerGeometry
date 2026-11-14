#include <afxwin.h>
#include "ChildView.h"

class CMainFrame : public CFrameWnd
{
    CChildView view;
    DECLARE_MESSAGE_MAP()

    afx_msg void OnTestF1() { view.SelectF1(); }
    afx_msg void OnTestF2() { view.SelectF2(); }
    afx_msg void OnTestF3() { view.SelectF3(); }

public:
    CMainFrame()
    {
        Create(nullptr, _T("Lab 3"),
               WS_OVERLAPPEDWINDOW, rectDefault, nullptr);

        CMenu* menu = new CMenu;
        menu->CreateMenu();
        CMenu tests;
        tests.CreatePopupMenu();
        tests.AppendMenu(MF_STRING, ID_TEST_F1, _T("F1"));
        tests.AppendMenu(MF_STRING, ID_TEST_F2, _T("F2"));
        tests.AppendMenu(MF_STRING, ID_TEST_F3, _T("F3"));
        menu->AppendMenu(MF_POPUP, (UINT_PTR)tests.Detach(), _T("Tests_F"));
        SetMenu(menu);
        menu->Detach();
        delete menu;

        view.Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE,
                    CRect(0, 0, 800, 600), this, 1000);
    }
};

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_COMMAND(ID_TEST_F1, &CMainFrame::OnTestF1)
    ON_COMMAND(ID_TEST_F2, &CMainFrame::OnTestF2)
    ON_COMMAND(ID_TEST_F3, &CMainFrame::OnTestF3)
END_MESSAGE_MAP()

class CMyPlot2DApp : public CWinApp
{
public:
    BOOL InitInstance() override
    {
        CWinApp::InitInstance();
        CMainFrame* frame = new CMainFrame;
        m_pMainWnd = frame;
        frame->ShowWindow(SW_SHOWMAXIMIZED);   // окно сразу на весь экран
        frame->UpdateWindow();
        return TRUE;
    }
};

CMyPlot2DApp theApp;
