#include "../lab5/BladeGeometry.h"

#include <cassert>
#include <cmath>
#include <iostream>

namespace
{
    void ExpectNear(double actual, double expected)
    {
        assert(std::fabs(actual - expected) < 1e-9);
    }

    void TestBladeCountBounds()
    {
        assert(IsValidBladeCount(2));
        assert(IsValidBladeCount(12));
        assert(!IsValidBladeCount(1));
        assert(!IsValidBladeCount(13));
    }

    void TestBladeHasFlatInnerBaseAndOutwardTip()
    {
        BladePoint vertices[3];
        GetBladeVertices(0.0, 42.0, 270.0, 12.0, vertices);

        ExpectNear(vertices[0].x, 42.0);
        ExpectNear(vertices[0].y, -12.0);
        ExpectNear(vertices[1].x, 42.0);
        ExpectNear(vertices[1].y, 12.0);
        ExpectNear(vertices[2].x, 270.0);
        ExpectNear(vertices[2].y, 0.0);
    }

    void TestBladeGeometryRotatesAroundCenter()
    {
        BladePoint vertices[3];
        GetBladeVertices(90.0, 42.0, 270.0, 12.0, vertices);

        ExpectNear(vertices[0].x, 12.0);
        ExpectNear(vertices[0].y, 42.0);
        ExpectNear(vertices[1].x, -12.0);
        ExpectNear(vertices[1].y, 42.0);
        ExpectNear(vertices[2].x, 0.0);
        ExpectNear(vertices[2].y, 270.0);
    }

    void TestAngularSpeedAdvancesClockwiseAndWraps()
    {
        ExpectNear(AdvanceClockwiseAngle(0.0, 90.0, 0.5), 315.0);
        ExpectNear(AdvanceClockwiseAngle(10.0, 720.0, 1.0), 10.0);
        ExpectNear(AdvanceClockwiseAngle(45.0, 0.0, 10.0), 45.0);
    }

    void TestCenterHitTarget()
    {
        assert(IsPointInsideCircle(100, 100, 100, 100, 12));
        assert(IsPointInsideCircle(112, 100, 100, 100, 12));
        assert(!IsPointInsideCircle(113, 100, 100, 100, 12));
    }
}

int main()
{
    TestBladeCountBounds();
    TestBladeHasFlatInnerBaseAndOutwardTip();
    TestBladeGeometryRotatesAroundCenter();
    TestAngularSpeedAdvancesClockwiseAndWraps();
    TestCenterHitTarget();
    std::cout << "All lab5 blade geometry tests passed\n";
}
