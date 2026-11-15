#include "stdafx.h"
#include "BladeGeometry.h"
#include <algorithm>
#include <cmath>

CRectD::CRectD(double l, double t, double r, double b)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}
//------------------------------------------------------------------------------
void CRectD::SetRectD(double l, double t, double r, double b)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}

//------------------------------------------------------------------------------
CSizeD CRectD::SizeD()
{
	CSizeD cz;
	cz.cx = fabs(right - left);
	cz.cy = fabs(top - bottom);
	return cz;
}

//----------------------------------------------------------------------------

CMatrix CreateTranslate2D(double dx, double dy)
// Формирует матрицу смещения на dx по оси X и на dy по оси Y
{
	CMatrix TM(3, 3);
	TM(0, 0) = 1; TM(0, 2) = dx;
	TM(1, 1) = 1;  TM(1, 2) = dy;
	TM(2, 2) = 1;
	return TM;
}

//------------------------------------------------------------------------------------
CMatrix CreateRotate2D(double fi)
// Формирует матрицу поворота на угол fi (fi > 0 — против часовой стрелки), fi в градусах
{
	double fg = fmod(fi, 360.0);
	double ff = (fg / 180.0) * pi;
	CMatrix RM(3, 3);
	RM(0, 0) = cos(ff); RM(0, 1) = -sin(ff);
	RM(1, 0) = sin(ff);  RM(1, 1) = cos(ff);
	RM(2, 2) = 1;
	return RM;
}


//------------------------------------------------------------------------------

CMatrix SpaceToWindow(CRectD& RS, CRect& RW)
// Преобразование координат из мировых в оконные
{
	CMatrix M(3, 3);
	CSize sz = RW.Size();
	int dwx = sz.cx;
	int dwy = sz.cy;
	CSizeD szd = RS.SizeD();

	double dsx = szd.cx;
	double dsy = szd.cy;

	double kx = (double)dwx / dsx;
	double ky = (double)dwy / dsy;

	M(0, 0) = kx;  M(0, 1) = 0;    M(0, 2) = (double)RW.left - kx * RS.left;
	M(1, 0) = 0;   M(1, 1) = -ky;  M(1, 2) = (double)RW.bottom + ky * RS.bottom;
	M(2, 0) = 0;   M(2, 1) = 0;    M(2, 2) = 1;
	return M;
}

//------------------------------------------------------------------------------

void SetMyMode(CDC& dc, CRectD& RS, CRect& RW)
// Устанавливает режим отображения MM_ANISOTROPIC
{
	double dsx = RS.right - RS.left;
	double dsy = RS.top - RS.bottom;
	double xsL = RS.left;
	double ysL = RS.bottom;

	int dwx = RW.right - RW.left;
	int dwy = RW.bottom - RW.top;
	int xwL = RW.left;
	int ywH = RW.bottom;

	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt((int)dsx, (int)dsy);
	dc.SetViewportExt(dwx, -dwy);
	dc.SetWindowOrg((int)xsL, (int)ysL);
	dc.SetViewportOrg(xwL, ywH);
}

CBlade::CBlade()
	: angleDegrees(0.0),
	  angularSpeedDegreesPerSecond(120.0),
	  bladeCount(4),
	  innerRadius(42.0),
	  outerRadius(270.0),
	  bladeHalfWidth(24.0),
	  hubRadius(30.0)
{
	RS.SetRectD(-300.0, 300.0, 300.0, -300.0);	// область в мировых координатах
	RW.SetRect(0, 0, 690, 640);				// область в окне (по умолчанию)
}

void CBlade::SetAngularSpeed(double speedDegreesPerSecond)
{
	if (speedDegreesPerSecond < 0.0)
		speedDegreesPerSecond = 0.0;
	if (speedDegreesPerSecond > 360.0)
		speedDegreesPerSecond = 360.0;
	angularSpeedDegreesPerSecond = speedDegreesPerSecond;
}

bool CBlade::SetBladeCount(int count)
{
	if (!IsValidBladeCount(count))
		return false;
	bladeCount = count;
	return true;
}

void CBlade::Advance(double elapsedSeconds)
{
	if (elapsedSeconds <= 0.0)
		return;
	angleDegrees = AdvanceClockwiseAngle(angleDegrees, angularSpeedDegreesPerSecond, elapsedSeconds);
}

bool CBlade::IsHubHit(CPoint point) const
{
	const int centerX = (RW.left + RW.right) / 2;
	const int centerY = (RW.top + RW.bottom) / 2;
	const double scaleX = (double)RW.Width() / (RS.right - RS.left);
	const double scaleY = (double)RW.Height() / (RS.top - RS.bottom);
	const double scale = (std::min)(scaleX, scaleY);
	const int hitRadius = (std::max)(16, (int)std::lround(hubRadius * scale));
	return IsPointInsideCircle(point.x, point.y, centerX, centerY, hitRadius);
}

void CBlade::Draw(CDC& dc)
{
	CPen redPen(PS_SOLID, 1, RGB(220, 45, 45));
	CPen bluePen(PS_SOLID, 1, RGB(45, 105, 210));
	CBrush redBrush(RGB(220, 45, 45));
	CBrush blueBrush(RGB(45, 105, 210));
	CPen* oldPen = dc.GetCurrentPen();
	CBrush* oldBrush = dc.GetCurrentBrush();

	for (int i = 0; i < bladeCount; ++i)
	{
		BladePoint vertices[3];
		const double bladeAngle = angleDegrees + 360.0 * i / bladeCount;
		GetBladeVertices(bladeAngle, innerRadius, outerRadius, bladeHalfWidth, vertices);

		CPoint points[3];
		points[0] = CPoint((int)std::lround(vertices[0].x), (int)std::lround(vertices[0].y));
		points[1] = CPoint((int)std::lround(vertices[1].x), (int)std::lround(vertices[1].y));
		points[2] = CPoint((int)std::lround(vertices[2].x), (int)std::lround(vertices[2].y));

		const bool even = (i % 2) == 0;
		dc.SelectObject(even ? &redPen : &bluePen);
		dc.SelectObject(even ? &redBrush : &blueBrush);
		dc.Polygon(points, 3);
	}

	CBrush hubBrush(RGB(0, 128, 0));
	CPen hubPen(PS_SOLID, 1, RGB(0, 96, 0));
	dc.SelectObject(&hubPen);
	dc.SelectObject(&hubBrush);
	const int hub = (int)std::lround(hubRadius);
	dc.Ellipse(-hub, -hub, hub, hub);

	dc.SelectObject(oldPen);
	dc.SelectObject(oldBrush);
}

void CBlade::GetRS(CRectD& RSX) const
// RSX - область, в которой отображается объект
{
	RSX.left = RS.left;
	RSX.top = RS.top;
	RSX.right = RS.right;
	RSX.bottom = RS.bottom;
}
