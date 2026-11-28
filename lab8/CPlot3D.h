#pragma once

#include <windows.h>
#include "SurfaceCore.h"

namespace lab8 {

class CPlot3D {
public:
    void Draw(HDC dc, const RECT& window_rect, SurfaceKind kind,
              double azimuth_degrees, double zenith_degrees) const;
};

}
