#pragma once
#include "LibChart2D.h"

// Окно CChildView
class CChildView : public CWnd
{

public:
	CChildView();

	
public:
	int m_Mode;          // Номер текущего теста 
	CPlot2D m_Graph;     // Объект нашего графика
	CMyPen m_PenLine;    // Перо графика / фигуры
	CMyPen m_PenAxis;    // Перо осей / окружности
	CRect m_RW;          // Область вывода на экране

	double MyF1(double x);
	double MyF2(double x);


public:
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	virtual ~CChildView();

protected:
	afx_msg void OnPaint();
	afx_msg void OnTestsF1();
	afx_msg void OnTestsF2();
	afx_msg void OnTestsF3();
	DECLARE_MESSAGE_MAP()
};