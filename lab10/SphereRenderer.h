#pragma once

#include <afxwin.h>

#include "CMatrix.h"
#include "SphereLightingCore.h"

void DrawLightSphere(CDC& dc, double Radius, CMatrix& PView, CMatrix& PSourceLight,
                     CRect RW, COLORREF Color, int Index);
