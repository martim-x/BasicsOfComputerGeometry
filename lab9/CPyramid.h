#pragma once

#include <afxwin.h>
#include <vector>

#include "CMatrix.h"
#include "LightingCore.h"

class CPyramid {
public:
    CPyramid();
    void ColorDraw(CDC& dc, CMatrix& PView, CMatrix& PLight, CRect& RW, COLORREF Color) const;
private:
    std::vector<lab9::Face> faces_;
};
