#pragma once

#include "CMatrix.h"

#include <string>
#include <vector>

namespace lab11 {

struct Point {
    double x;
    double y;
};

std::vector<Point> AssignmentLagrangeNodes();
std::vector<Point> AssignmentLagrangeCurve(const std::vector<Point>& nodes);
std::vector<Point> AssignmentQuadraticControls();
std::vector<Point> AssignmentCubicControls();
std::vector<Point> BezierPoints(const std::vector<Point>& control_points, int segments);
void WritePointFile(const std::string& path, const std::vector<Point>& points);

}

double Lagr(CMatrix& X, CMatrix& Y, double x);
void Bezier(CMatrix& X, CMatrix& Y, CMatrix& XB, CMatrix& YB, int M);
