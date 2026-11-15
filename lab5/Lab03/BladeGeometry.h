#pragma once

struct BladePoint
{
    double x;
    double y;
};

bool IsValidBladeCount(int count);
void GetBladeVertices(double angleDegrees, double innerRadius, double outerRadius,
    double halfWidth, BladePoint (&vertices)[3]);
double AdvanceClockwiseAngle(double angleDegrees, double angularSpeedDegreesPerSecond,
    double elapsedSeconds);
bool IsPointInsideCircle(int x, int y, int centerX, int centerY, int radius);
