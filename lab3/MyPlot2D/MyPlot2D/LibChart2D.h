#pragma once
#include "framework.h"
#include "CMatrix.h"

// Структура вещественного прямоугольника в МСК
struct CRectD
{
	double left;
	double top;
	double right;
	double bottom;

	CRectD() { left = top = right = bottom = 0.0; }
	CRectD(double l, double t, double r, double b) : left(l), top(t), right(r), bottom(b) {}
	void SetRectD(double l, double t, double r, double b)
	{
		left = l; top = t; right = r; bottom = b;
	}
	double Width() const { return fabs(right - left); }
	double Height() const { return fabs(top - bottom); }
};


struct CMyPen
{
	int PenStyle;       
	int PenWidth;      
	COLORREF PenColor;  

	CMyPen()
	{
		PenStyle = PS_SOLID;
		PenWidth = 1;
		PenColor = RGB(0, 0, 0);
	}

	void Set(int PS, int PW, COLORREF PC)
	{
		PenStyle = PS;
		PenWidth = PW;
		PenColor = PC;
	}
};

// Функция пересчета координат из мировых в оконные
CMatrix SpaceToWindow(CRectD& rs, CRect& rw);

// Класс для отображения графика зависимости Y = F(X)
class CPlot2D
{
	CMatrix X;         // Массив аргументов
	CMatrix Y;         // Массив значений функции
	CMatrix K;         // Матрица пересчета координат 3x3
	CRect RW;          // Прямоугольник в оконных координатах (пиксели)
	CRectD RS;         // Прямоугольник в мировой системе координат
	CMyPen PenLine;    // Перо для рисования графика
	CMyPen PenAxis;    // Перо для координатных осей

public:
	CPlot2D() { K.RedimMatrix(3, 3); }

	void SetParams(CMatrix& XX, CMatrix& YY, CRect& RWX);
	void SetWindowRect(CRect& RWX);
	void GetWindowCoords(double xs, double ys, int& xw, int& yw);
	void SetPenLine(CMyPen& PLine);
	void SetPenAxis(CMyPen& PAxis);
	void Draw(CDC& dc, int Ind1, int Ind2);
};