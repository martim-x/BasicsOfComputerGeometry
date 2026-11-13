#include "pch.h"
#include "LibChart2D.h"


// Vw = T * V, где T - матрица 3x3
CMatrix SpaceToWindow(CRectD& rs, CRect& rw)
{
	CMatrix M(3, 3);

	// Размеры окна в пикселях
	double dwx = rw.Width();
	double dwy = rw.Height();

	// Размеры области в мировых координатах
	double dsx = rs.Width();
	double dsy = rs.Height();

	
	double kx = dwx / dsx;
	double ky = dwy / dsy;

	double tx = (double)rw.left - kx * rs.left;// tx = xLw - kx * xLs
	double ty = (double)rw.bottom + ky * rs.bottom;// ty = yHw + ky * yLs

	//kx   0  tx 
	//0 -ky   ty 
	//0   0    1 
	M(0, 0) = kx;   M(0, 1) = 0.0;  M(0, 2) = tx;
	M(1, 0) = 0.0;  M(1, 1) = -ky;  M(1, 2) = ty;
	M(2, 0) = 0.0;  M(2, 1) = 0.0;  M(2, 2) = 1.0;

	return M;
}

void CPlot2D::SetParams(CMatrix& XX, CMatrix& YY, CRect& RWX)
{
	X = XX;
	Y = YY;

	// Границы области функции в мировой системе координат
	double x_min = X.MinElement();
	double x_max = X.MaxElement();
	double y_min = Y.MinElement();
	double y_max = Y.MaxElement();

	RS.SetRectD(x_min, y_max, x_max, y_min);
	RW = RWX;

	// Пересчитываем матрицу перехода
	K = SpaceToWindow(RS, RW);
}

void CPlot2D::SetWindowRect(CRect& RWX)
{
	RW = RWX;
	K = SpaceToWindow(RS, RW);
}

// Пересчет точки (xs, ys) в (xw, yw) с помощью матрицы K
void CPlot2D::GetWindowCoords(double xs, double ys, int& xw, int& yw)
{
	CMatrix V(3); // Вектор-столбец xs, ys, 1 ^T
	V(0) = xs;
	V(1) = ys;
	V(2) = 1.0;

	CMatrix W = K * V; // W = K * V
	xw = (int)W(0);
	yw = (int)W(1);
}

void CPlot2D::SetPenLine(CMyPen& PLine)
{
	PenLine = PLine;
}

void CPlot2D::SetPenAxis(CMyPen& PAxis)
{
	PenAxis = PAxis;
}

// Отрисовка:
// Ind1 = 1 - рисовать рамку области графика
// Ind2 = 1 - рисовать координатные оси X и Y
void CPlot2D::Draw(CDC& dc, int Ind1, int Ind2)
{
	//Рисуем прямоугольник области
	if (Ind1 == 1)
	{
		dc.Rectangle(RW);
	}

	//Рисуем оси координат (если они попадают в диапазон)
	if (Ind2 == 1)
	{
		CPen axisPen(PenAxis.PenStyle, PenAxis.PenWidth, PenAxis.PenColor);
		CPen* pOldPen = dc.SelectObject(&axisPen);

		int xw, yw;

		// Ось Y (проходит через x = 0), если 0 между x_min и x_max
		if (RS.left * RS.right <= 0)
		{
			GetWindowCoords(0.0, RS.top, xw, yw); // Верхняя точка оси Y
			dc.MoveTo(xw, yw);
			GetWindowCoords(0.0, RS.bottom, xw, yw); // Нижняя точка оси Y
			dc.LineTo(xw, yw);
		}

		// Ось X (проходит через y = 0), если 0 между y_min и y_max
		if (RS.bottom * RS.top <= 0)
		{
			GetWindowCoords(RS.left, 0.0, xw, yw); // Левая точка оси X
			dc.MoveTo(xw, yw);
			GetWindowCoords(RS.right, 0.0, xw, yw); // Правая точка оси X
			dc.LineTo(xw, yw);
		}

		dc.SelectObject(pOldPen);
	}

	//Рисуем сам график
	if (X.rows() > 1)
	{
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = PenLine.PenColor;
		lb.lbHatch = 0;

		CPen linePen;
		if (PenLine.PenStyle != PS_SOLID)
		{
			linePen.CreatePen(PS_GEOMETRIC | PenLine.PenStyle, PenLine.PenWidth, &lb);
		}
		else
		{
			linePen.CreatePen(PenLine.PenStyle, PenLine.PenWidth, PenLine.PenColor);
		}
		CPen* pOldPen = dc.SelectObject(&linePen);

		int xw, yw;
		GetWindowCoords(X(0), Y(0), xw, yw);
		dc.MoveTo(xw, yw);

		for (int i = 1; i < X.rows(); i++)
		{
			GetWindowCoords(X(i), Y(i), xw, yw);
			dc.LineTo(xw, yw);
		}

		dc.SelectObject(pOldPen);
	}
}