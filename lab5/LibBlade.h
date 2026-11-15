#ifndef LIBPLANETS
#define LIBPLANETS 1
const double pi = 3.14159;


struct CSizeD
{
	double cx;
	double cy;
};
//-------------------------------------------------------------------------------
struct CRectD
{
	double left;
	double top;
	double right;
	double bottom;
	CRectD() { left = top = right = bottom = 0; };
	CRectD(double l, double t, double r, double b);
	void SetRectD(double l, double t, double r, double b);
	CSizeD SizeD();		// размеры прямоугольника (ширина, высота) в мировых координатах
};
//-------------------------------------------------------------------------------

CMatrix CreateTranslate2D(double dx, double dy);
CMatrix CreateRotate2D(double fi);
CMatrix SpaceToWindow(CRectD& rs, CRect& rw);
void SetMyMode(CDC& dc, CRectD& RS, CRect& RW);



class CBlade
{
	double angleDegrees;					// текущий угол поворота фигуры
	double angularSpeedDegreesPerSecond;	// угловая скорость, градусов в секунду
	int bladeCount;						// количество лопастей
	double innerRadius;					// расстояние от центра до плоского основания
	double outerRadius;					// расстояние от центра до острия лопасти
	double bladeHalfWidth;				// половина ширины основания лопасти
	double hubRadius;					// радиус центральной втулки в мировых координатах
	CRect RW;							// область отображения в окне
	CRectD RS;							// область в мировых координатах
public:
	CBlade();
	void SetAngularSpeed(double speedDegreesPerSecond);
	double GetAngularSpeed() const { return angularSpeedDegreesPerSecond; };
	bool SetBladeCount(int count);
	int GetBladeCount() const { return bladeCount; };
	void Advance(double elapsedSeconds);
	bool IsHubHit(CPoint point) const;
	void GetRS(CRectD& RSX) const;
	CRect GetRW() const { return RW; };
	void SetRW(const CRect& rw) { RW = rw; };
	void Draw(CDC& dc);
};


#endif
