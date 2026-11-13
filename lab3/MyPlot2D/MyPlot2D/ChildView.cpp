#include "pch.h"
#include "framework.h"
#include "MyPlot2D.h"
#include "ChildView.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const double PI = 3.14159265358979323846;

// Конструктор
CChildView::CChildView()
{
	m_Mode = 0; // При старте ничего не выбрано
}

CChildView::~CChildView()
{
}

// Привязка обработчиков меню к кнопкам
BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_TESTS_F1, &CChildView::OnTestsF1)
	ON_COMMAND(ID_TESTS_F2, &CChildView::OnTestsF2)
	ON_COMMAND(ID_TESTS_F3, &CChildView::OnTestsF3)
END_MESSAGE_MAP()

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) //настройка окна перерерисовка при растягивании
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return TRUE;
}


double CChildView::MyF1(double x)
{
	if (fabs(x) < 1e-7) return 1.0; // Предел sin(x)/x при x->0 равен 1
	return sin(x) / x;
}

double CChildView::MyF2(double x)
{
	return sqrt(fabs(x)) * sin(x);
}

// Пункт меню Tests_F  F1
void CChildView::OnTestsF1()
{
	double xL = -3.0 * PI;  // Левая граница
	double xH = 3.0 * PI;
	double dx = PI / 36.0; // Шаг 
	int N = (int)((xH - xL) / dx);

	CMatrix X(N + 1);
	CMatrix Y(N + 1);

	for (int i = 0; i <= N; i++)
	{
		X(i) = xL + i * dx;
		Y(i) = MyF1(X(i));
	}

	// Перо графика: толщина 1, красный, сплошная
	m_PenLine.Set(PS_SOLID, 1, RGB(255, 0, 0));
	// Перо осей: толщина 2, синий
	m_PenAxis.Set(PS_SOLID, 2, RGB(0, 0, 255));

	// Центрируем область графика в окне
	CRect clientRect;
	GetClientRect(&clientRect);
	int w = 550;
	int h = 400;
	int left = (clientRect.Width() - w) / 2;
	int top = (clientRect.Height() - h) / 2;
	m_RW.SetRect(left, top, left + w, top + h);

	m_Graph.SetParams(X, Y, m_RW);
	m_Graph.SetPenLine(m_PenLine);
	m_Graph.SetPenAxis(m_PenAxis);

	m_Mode = 1;
	Invalidate(); // Перерисовать окно
}

// Пункт меню Tests_F  F2
void CChildView::OnTestsF2()
{
	double xL = -4.0 * PI;
	double xH = 4.0 * PI;
	double dx = PI / 36.0;
	int N = (int)((xH - xL) / dx);

	CMatrix X(N + 1);
	CMatrix Y(N + 1);

	for (int i = 0; i <= N; i++)
	{
		X(i) = xL + i * dx;
		Y(i) = MyF2(X(i));
	}

	// Перо графика: толщина 3, красный, штрих-пунктирная 
	m_PenLine.Set(PS_DASHDOT, 3, RGB(255, 0, 0));
	// Перо осей: толщина 2, черный
	m_PenAxis.Set(PS_SOLID, 2, RGB(0, 0, 0));

	
	CRect clientRect;
	GetClientRect(&clientRect);
	int w = 550;
	int h = 400;
	int left = (clientRect.Width() - w) / 2;
	int top = (clientRect.Height() - h) / 2;
	m_RW.SetRect(left, top, left + w, top + h);

	m_Graph.SetParams(X, Y, m_RW);
	m_Graph.SetPenLine(m_PenLine);
	m_Graph.SetPenAxis(m_PenAxis);

	m_Mode = 2;
	Invalidate(); 
}

// Пункт меню Tests_F F3 (Восьмиугольник, вписанный в круг R = 10)
void CChildView::OnTestsF3()
{
	m_Mode = 3;
	Invalidate(); 
}

// Отрисовка в окне
void CChildView::OnPaint()
{
	CPaintDC dc(this);

	if (m_Mode == 1 || m_Mode == 2)
	{
		// Отображаем график с осями и рамкой (Ind1=1, Ind2=1)
		m_Graph.Draw(dc, 1, 1);
	}
	else if (m_Mode == 3)
	{
		// Область окна квадрат по центру
		CRect clientRect;
		GetClientRect(&clientRect);
		int side = min(clientRect.Width(), clientRect.Height()) - 100;
		if (side < 100) side = 100;

		int left = (clientRect.Width() - side) / 2;
		int top = (clientRect.Height() - side) / 2;
		CRect RW(left, top, left + side, top + side);

		// Рамка квадратной области
		dc.Rectangle(RW);

		// В мировых координатах область круга R = 10: [-10, 10] x [-10, 10]
		CRectD RS(-10.0, 10.0, 10.0, -10.0);

		// Матрица пересчета из МСК в окно
		CMatrix K = SpaceToWindow(RS, RW);

		//Рисуем окружность толщина 2, синий, сплошная
		CPen penCircle(PS_SOLID, 2, RGB(0, 0, 255));
		CPen* pOldPen = dc.SelectObject(&penCircle);

		// Окружность вписывается ровно в область RW
		dc.SelectStockObject(NULL_BRUSH); // Прозрачная кисть, чтобы не закрашивать фигуру
		dc.Ellipse(RW);

		// Рисуем правильный восьмиугольник толщина 3, красный, сплошная
		CPen penFigure(PS_SOLID, 3, RGB(255, 0, 0));
		dc.SelectObject(&penFigure);

		const int numVertices = 8;
		for (int i = 0; i <= numVertices; i++)
		{
			// Угол вершины
			double angle = i * 2.0 * PI / numVertices;
			// Мировые координаты вершины
			double xs = 10.0 * cos(angle); 
			double ys = 10.0 * sin(angle);

			// Пересчет точки через матрицу K: W = K * V
			CMatrix V(3);
			V(0) = xs;
			V(1) = ys;
			V(2) = 1.0;
			CMatrix W = K * V;
			int xw = (int)W(0);
			int yw = (int)W(1);

			if (i == 0)
				dc.MoveTo(xw, yw);
			else
				dc.LineTo(xw, yw);
		}

		dc.SelectObject(pOldPen);
	}
}