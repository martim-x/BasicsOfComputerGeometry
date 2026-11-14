#include "Geometry.h"

void CMatrix::RedimMatrix(int r, int c)
{
    rows = r;
    cols = c;
    data.assign(rows * cols, 0.0);
}

CMatrix SpaceToWindow(CRectD& rs, CRect& rw)
{
    CMatrix k;
    k.RedimMatrix(3, 3);
    const double sx = (rw.right - rw.left) / (rs.right - rs.left);
    const double sy = (rw.bottom - rw.top) / (rs.bottom - rs.top);
    k(0, 0) = sx;
    k(1, 1) = sy;
    k(2, 2) = 1.0;
    k(0, 2) = rw.left - sx * rs.left;
    k(1, 2) = rw.top - sy * rs.top;
    return k;
}
