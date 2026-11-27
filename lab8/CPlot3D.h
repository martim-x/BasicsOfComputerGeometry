#pragma once

#include <afxwin.h>
#include "SurfaceCore.h"

namespace lab8 {

class CPlot3D {
public:
    void Draw(CDC& dc, const CRect& window_rect, SurfaceKind kind,
              double azimuth_degrees, double zenith_degrees) const;
};

}
