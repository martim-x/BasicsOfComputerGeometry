#include "CMatrix.h"
#include "CPyramid.h"
#include <afxwin.h>
#include <windows.h>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#define ID_Dr1 2002
#define ID_Dr 2003
#define ID_AngF 2004
#define ID_AngD 2005


class CMainWin : public CFrameWnd
{
public:
	int ang1 = 10;
	int ang2 = 315;
	int ang3 = 45;

	CMainWin()
	{
		Create(NULL, L"Lab7");
		WinRect.SetRect(50, 50, 650, 650);
		Index = 0;
		PView.RedimMatrix(3);
	}
	CMenu *menu;
	CRect WinRect;
	CPyramid PIR;
	CMatrix PView;
	int Index;
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void Dr1();
	void Dr();
	void AngF();
	void AngD();
	DECLARE_MESSAGE_MAP();
};
class CMyApp : public CWinApp
{
public:
	CMyApp() {};
	virtual BOOL InitInstance()
	{
		m_pMainWnd = new CMainWin();
		m_pMainWnd->ShowWindow(SW_SHOW);
		return TRUE;
	}
};
BEGIN_MESSAGE_MAP(CMainWin, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_COMMAND(ID_Dr1, Dr1)
	ON_COMMAND(ID_Dr, Dr)
	ON_COMMAND(ID_AngF, AngF)
	ON_COMMAND(ID_AngD, AngD)
	ON_WM_SIZE()
END_MESSAGE_MAP()

afx_msg void CMainWin::OnPaint()
{
	CPaintDC dc(this);
	CRect client;
	GetClientRect(&client);
	dc.FillSolidRect(client, RGB(255, 255, 255));
	if (Index == 1) PIR.Draw(dc, PView, WinRect);
	if (Index == 2) PIR.Draw1(dc, PView, WinRect);

}

afx_msg int CMainWin::OnCreate(LPCREATESTRUCT)
{
	menu = new CMenu();
	menu->CreateMenu();
	this->menu->AppendMenu(MF_STRING, ID_Dr, _T("Только видимые"));
	this->menu->AppendMenu(MF_STRING, ID_Dr1, _T("Все грани"));
	this->menu->AppendMenu(MF_STRING, ID_AngF, _T("Текущие углы камеры"));
	this->menu->AppendMenu(MF_STRING, ID_AngD, _T("По умолчанию"));

	SetMenu(menu);
	return 0;
}

void CMainWin::Dr1()
{

	PView(0) = ang1;
	PView(1) = ang2;
	PView(2) = ang3;
	Index = 1;
	Invalidate();
}

void CMainWin::Dr()
{

	PView(0) = ang1;
	PView(1) = ang2;
	PView(2) = ang3;
	Index = 2;
	Invalidate();
}

void CMainWin::AngF()
{
	std::ifstream file("angles.txt");
	std::string line;
	std::vector<std::string> lines;

	if (file.is_open()) {
		while (std::getline(file, line)) {
			if (!line.empty())
				lines.push_back(line);
		}
		file.close();
	}

	if (lines.size() < 2)
		return;

	try
	{
		ang2 = stoi(lines[0]);
		ang3 = stoi(lines[1]);
	}
	catch (const std::exception&)
	{
		return;
	}
	PView(1) = ang2;
	PView(2) = ang3;

	Invalidate();
}


void CMainWin::AngD()
{
	ang2 = 315;
	ang3 = 45;
	PView(1) = ang2;
	PView(2) = ang3;
	Invalidate();
}

afx_msg void CMainWin::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	WinRect.SetRect((int)(cx * 0.1), (int)(cy * 0.1), (int)(cx * 0.9), (int)(cy * 0.9));
}

CMyApp theApp;