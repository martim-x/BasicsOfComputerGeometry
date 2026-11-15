#include "BladeGeometry.h"

#include <cmath>

bool IsValidBladeCount(int count)
{
    return count >= 2 && count <= 12;
}

void GetBladeVertices(double angleDegrees, double innerRadius, double outerRadius,
    double halfWidth, BladePoint (&vertices)[3])
{
    const double radians = angleDegrees * 3.14159265358979323846 / 180.0;
    const double directionX = std::cos(radians);
    const double directionY = std::sin(radians);
    const double normalX = -directionY;
    const double normalY = directionX;

    vertices[0] = {
        innerRadius * directionX - halfWidth * normalX,
        innerRadius * directionY - halfWidth * normalY
    };
    vertices[1] = {
        innerRadius * directionX + halfWidth * normalX,
        innerRadius * directionY + halfWidth * normalY
    };
    vertices[2] = { outerRadius * directionX, outerRadius * directionY };
}

double AdvanceClockwiseAngle(double angleDegrees, double angularSpeedDegreesPerSecond,
    double elapsedSeconds)
{
    double angle = std::fmod(angleDegrees - angularSpeedDegreesPerSecond * elapsedSeconds, 360.0);
    if (angle < 0.0)
        angle += 360.0;
    return angle;
}

bool IsPointInsideCircle(int x, int y, int centerX, int centerY, int radius)
{
    const long long dx = static_cast<long long>(x) - centerX;
    const long long dy = static_cast<long long>(y) - centerY;
    const long long r = radius;
    return dx * dx + dy * dy <= r * r;
}
