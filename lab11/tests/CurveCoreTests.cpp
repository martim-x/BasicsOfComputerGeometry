#include "CurveCore.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

CMatrix vector_of(std::initializer_list<double> values)
{
    CMatrix result(static_cast<int>(values.size()));
    int index = 0;
    for (double value : values) {
        result(index++) = value;
    }
    return result;
}

bool near(double actual, double expected, double tolerance = 1e-10)
{
    return std::abs(actual - expected) <= tolerance;
}

void test_lagrange_interpolates_nodes_and_endpoints()
{
    CMatrix x = vector_of({-1.0, 0.0, 1.0});
    CMatrix y = vector_of({1.0, 0.0, 1.0});
    assert(near(Lagr(x, y, -1.0), 1.0));
    assert(near(Lagr(x, y, 0.0), 0.0));
    assert(near(Lagr(x, y, 1.0), 1.0));
}

void test_lagrange_evaluates_interior_values()
{
    CMatrix x = vector_of({-1.0, 0.0, 1.0});
    CMatrix y = vector_of({1.0, 0.0, 1.0});
    assert(near(Lagr(x, y, 0.5), 0.25));
    assert(near(Lagr(x, y, -0.5), 0.25));
}

void test_lagrange_rejects_repeated_abscissas()
{
    CMatrix x = vector_of({0.0, 1.0, 1.0});
    CMatrix y = vector_of({2.0, 3.0, 4.0});
    bool threw = false;
    try {
        static_cast<void>(Lagr(x, y, 0.5));
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
}

void test_lagrange_rejects_out_of_range_input()
{
    CMatrix x = vector_of({0.0, 1.0});
    CMatrix y = vector_of({2.0, 3.0});
    bool threw = false;
    try {
        static_cast<void>(Lagr(x, y, 1.1));
    } catch (const std::out_of_range&) {
        threw = true;
    }
    assert(threw);
}

void test_bezier_includes_endpoints_and_m_plus_one_samples()
{
    CMatrix x = vector_of({0.0, 2.0});
    CMatrix y = vector_of({1.0, 5.0});
    CMatrix xb;
    CMatrix yb;
    Bezier(x, y, xb, yb, 4);
    assert(xb.rows() == 5 && yb.rows() == 5);
    assert(near(xb(0), 0.0) && near(yb(0), 1.0));
    assert(near(xb(4), 2.0) && near(yb(4), 5.0));
    assert(near(xb(2), 1.0) && near(yb(2), 3.0));
}

void test_bezier_uses_geometric_de_casteljau_construction()
{
    CMatrix x = vector_of({0.0, 1.0, 2.0});
    CMatrix y = vector_of({0.0, 2.0, 0.0});
    CMatrix xb;
    CMatrix yb;
    Bezier(x, y, xb, yb, 2);
    assert(xb.rows() == 3 && yb.rows() == 3);
    assert(near(xb(1), 1.0));
    assert(near(yb(1), 1.0));
}

void test_bezier_rejects_invalid_segment_count()
{
    CMatrix x = vector_of({0.0, 1.0});
    CMatrix y = vector_of({0.0, 1.0});
    CMatrix xb;
    CMatrix yb;
    bool threw = false;
    try {
        Bezier(x, y, xb, yb, 0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
}

}

void test_assignment_lagrange_nodes_and_sample_count()
{
    constexpr double pi_value = 3.14159265358979323846;
    const std::vector<lab11::Point> nodes = lab11::AssignmentLagrangeNodes();
    assert(nodes.size() == 5);
    assert(near(nodes.front().x, 0.0));
    assert(near(nodes.back().x, pi_value));
    CMatrix x(5);
    CMatrix y(5);
    for (int i = 0; i < 5; ++i) {
        x(i) = nodes[static_cast<std::size_t>(i)].x;
        y(i) = nodes[static_cast<std::size_t>(i)].y;
        assert(near(y(i), std::pow(2.0 + std::cos(x(i)), std::sin(2.0 * x(i))), 1e-12));
    }
    for (int i = 0; i < 5; ++i) {
        assert(near(Lagr(x, y, x(i)), y(i), 1e-12));
    }
    const std::vector<lab11::Point> curve = lab11::AssignmentLagrangeCurve(nodes);
    assert(curve.size() == 17);
    assert(near(curve.front().x, 0.0));
    assert(near(curve.back().x, pi_value));
    assert(near(curve.front().y, nodes.front().y));
    assert(near(curve.back().y, nodes.back().y));
    for (const lab11::Point& point : curve) {
        assert(std::isfinite(point.x) && std::isfinite(point.y));
    }
}

void test_mathcad_export_writes_two_numeric_columns()
{
    const char* path = "/tmp/lab11-mathcad-export-test.txt";
    const std::vector<lab11::Point> points{{0.25, 1.5}, {2.0, -3.25}};
    lab11::WritePointFile(path, points);
    std::ifstream input(path);
    double x0 = 0.0;
    double y0 = 0.0;
    double x1 = 0.0;
    double y1 = 0.0;
    if (!(input >> x0 >> y0 >> x1 >> y1)) {
        assert(false);
    }
    std::string extra;
    assert(!(input >> extra));
    assert(near(x0, 0.25) && near(y0, 1.5));
    assert(near(x1, 2.0) && near(y1, -3.25));
    std::remove(path);
}

int main()
{
    test_lagrange_interpolates_nodes_and_endpoints();
    test_lagrange_evaluates_interior_values();
    test_lagrange_rejects_repeated_abscissas();
    test_lagrange_rejects_out_of_range_input();
    test_bezier_includes_endpoints_and_m_plus_one_samples();
    test_bezier_uses_geometric_de_casteljau_construction();
    test_bezier_rejects_invalid_segment_count();
    test_assignment_lagrange_nodes_and_sample_count();
    test_mathcad_export_writes_two_numeric_columns();
    std::cout << "Lab 11 curve core tests: PASS\n";
    return 0;
}
