// ChildView.cpp: реализация класса CChildView
//

#include "stdafx.h"
#include "lab5.h"
#include "ChildView.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
	: IsData(0), m_isAnimating(false), m_lastTick(0)
{
}

CChildView::~CChildView()
{
	if (m_isAnimating && ::IsWindow(m_hWnd))
		KillTimer(TimerAnimation);
}

// Реализация карты сообщений
BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	// сообщения меню выбора
	ON_COMMAND(ID_LAB_BLADE, &CChildView::OnLabpBlade)
END_MESSAGE_MAP()



// Обработчики сообщений CChildView

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return TRUE;
}

int CChildView::OnCreate(LPCREATESTRUCT createStruct)
{
	if (CWnd::OnCreate(createStruct) == -1)
		return -1;

	const DWORD labelStyle = WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE;
	const CRect initialRect(0, 0, 0, 0);
	if (!m_speedLabel.Create(_T("Угловая скорость, °/с"), labelStyle, initialRect, this, SpeedLabelId) ||
		!m_speedValue.Create(_T("120°/с"), labelStyle | SS_RIGHT, initialRect, this, SpeedValueId) ||
		!m_bladeCountLabel.Create(_T("Количество лопастей"), labelStyle, initialRect, this, BladeCountLabelId) ||
		!m_bladeCountValue.Create(_T("4"), labelStyle | SS_RIGHT, initialRect, this, BladeCountValueId) ||
		!m_statusLabel.Create(_T("Нажмите на центральную втулку, чтобы запустить вращение"),
			labelStyle, initialRect, this, StatusLabelId))
		return -1;

	const DWORD sliderStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | TBS_HORZ | TBS_AUTOTICKS;
	if (!m_speedSlider.Create(sliderStyle, initialRect, this, SpeedSliderId) ||
		!m_bladeCountSlider.Create(sliderStyle, initialRect, this, BladeCountSliderId))
		return -1;

	m_speedSlider.SetRange(0, 360);
	m_speedSlider.SetPageSize(30);
	m_speedSlider.SetTicFreq(60);
	m_speedSlider.SetPos((int)Blade.GetAngularSpeed());

	m_bladeCountSlider.SetRange(2, 12);
	m_bladeCountSlider.SetPageSize(1);
	m_bladeCountSlider.SetTicFreq(1);
	m_bladeCountSlider.SetPos(Blade.GetBladeCount());

	UpdateControlValues();
	LayoutControls();
	return 0;
}

void CChildView::LayoutControls()
{
	if (!::IsWindow(m_speedSlider.GetSafeHwnd()) || !::IsWindow(m_bladeCountSlider.GetSafeHwnd()))
		return;

	CRect client;
	GetClientRect(&client);

	const int margin = 12;
	const int labelWidth = 155;
	const int valueWidth = 72;
	const int sliderLeft = margin + labelWidth + 6;
	const int sliderWidth = (std::max)(80, client.Width() - sliderLeft - valueWidth - margin);
	const int valueLeft = sliderLeft + sliderWidth + 6;

	m_speedLabel.MoveWindow(margin, 6, labelWidth, 24);
	m_speedSlider.MoveWindow(sliderLeft, 4, sliderWidth, 27);
	m_speedValue.MoveWindow(valueLeft, 6, valueWidth, 24);

	m_bladeCountLabel.MoveWindow(margin, 35, labelWidth, 24);
	m_bladeCountSlider.MoveWindow(sliderLeft, 33, sliderWidth, 27);
	m_bladeCountValue.MoveWindow(valueLeft, 35, valueWidth, 24);

	m_statusLabel.MoveWindow(margin, 64, (std::max)(0, client.Width() - 2 * margin), 22);

	// область рисунка — квадрат по центру оставшейся части окна
	const int drawTop = 94;
	const int drawMargin = 16;
	const int availableWidth = (std::max)(0, client.Width() - 2 * drawMargin);
	const int availableHeight = (std::max)(0, client.Height() - drawTop - drawMargin);
	const int side = (std::max)(0, (std::min)(availableWidth, availableHeight));
	const int left = (client.Width() - side) / 2;
	const int top = drawTop + (availableHeight - side) / 2;

	Blade.SetRW(CRect(left, top, left + side, top + side));
	Invalidate(FALSE);
}

void CChildView::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type, cx, cy);
	LayoutControls();
}

void CChildView::UpdateControlValues()
{
	CString speedText;
	speedText.Format(_T("%d°/с"), m_speedSlider.GetPos());
	m_speedValue.SetWindowText(speedText);

	CString countText;
	countText.Format(_T("%d"), m_bladeCountSlider.GetPos());
	m_bladeCountValue.SetWindowText(countText);

	Blade.SetAngularSpeed((double)m_speedSlider.GetPos());
	Blade.SetBladeCount(m_bladeCountSlider.GetPos());
	Invalidate(FALSE);
}

void CChildView::OnHScroll(UINT code, UINT pos, CScrollBar* scrollBar)
{
	if (scrollBar != nullptr)
	{
		const int controlId = scrollBar->GetDlgCtrlID();
		if (controlId == SpeedSliderId || controlId == BladeCountSliderId)
			UpdateControlValues();
	}
	CWnd::OnHScroll(code, pos, scrollBar);
}

void CChildView::OnPaint()
{
	CPaintDC dc(this);

	CRect client;
	GetClientRect(&client);
	dc.FillSolidRect(client, RGB(255, 255, 255));

	if (IsData == 1)
	{
		CRectD world;
		Blade.GetRS(world);
		CRect window = Blade.GetRW();
		SetMyMode(dc, world, window);	// Устанавливает режим отображения MM_ANISOTROPIC
		Blade.Draw(dc);
		dc.SetMapMode(MM_TEXT);		// Устанавливает режим отображения MM_TEXT
	}
}

void CChildView::OnLabpBlade()
{
	if (m_isAnimating)
	{
		KillTimer(TimerAnimation);
		m_isAnimating = false;
	}
	IsData = 1;
	UpdateControlValues();
	m_statusLabel.SetWindowText(_T("Нажмите на центральную втулку, чтобы запустить вращение"));
	Invalidate(FALSE);
}

void CChildView::ToggleAnimation()
{
	if (IsData != 1)
		return;

	if (m_isAnimating)
	{
		KillTimer(TimerAnimation);
		m_isAnimating = false;
		m_statusLabel.SetWindowText(_T("Вращение остановлено. Нажмите на втулку, чтобы продолжить"));
		return;
	}

	m_lastTick = ::GetTickCount();
	if (SetTimer(TimerAnimation, 16, nullptr) != 0)
	{
		m_isAnimating = true;
		m_statusLabel.SetWindowText(_T("Вращение включено. Нажмите на втулку, чтобы остановить"));
	}
}

void CChildView::OnTimer(UINT_PTR eventId)
{
	if (eventId == TimerAnimation && m_isAnimating)
	{
		const DWORD now = ::GetTickCount();
		const DWORD elapsedMs = now - m_lastTick;
		m_lastTick = now;

		Blade.Advance((double)elapsedMs / 1000.0);
		Invalidate(FALSE);
		return;
	}
	CWnd::OnTimer(eventId);
}

void CChildView::OnLButtonDown(UINT flags, CPoint point)
{
	// одиночный щелчок по центральному шарику включает и выключает вращение
	if (Blade.IsHubHit(point))
		ToggleAnimation();
	CWnd::OnLButtonDown(flags, point);
}
