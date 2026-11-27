#include "CurveCore.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <utility>

CMatrix::CMatrix(int size)
{
    RedimMatrix(size);
}

int CMatrix::rows() const noexcept
{
    return static_cast<int>(values_.size());
}

int CMatrix::cols() const noexcept
{
    return 1;
}

double& CMatrix::operator()(int index)
{
    return values_.at(static_cast<std::size_t>(index));
}

double CMatrix::operator()(int index) const
{
    return values_.at(static_cast<std::size_t>(index));
}

void CMatrix::RedimMatrix(int size)
{
    if (size < 0) {
        throw std::invalid_argument("vector size must not be negative");
    }
    values_.assign(static_cast<std::size_t>(size), 0.0);
}

namespace {

void validate_control_points(const CMatrix& x, const CMatrix& y)
{
    if (x.rows() != y.rows() || x.rows() < 2) {
        throw std::invalid_argument("Bezier requires matching vectors with at least two points");
    }
    for (int i = 0; i < x.rows(); ++i) {
        if (!std::isfinite(x(i)) || !std::isfinite(y(i))) {
            throw std::invalid_argument("Bezier control points must be finite");
        }
    }
}

lab11::Point evaluate_bezier(const std::vector<lab11::Point>& controls, double t)
{
    std::vector<lab11::Point> level = controls;
    for (std::size_t remaining = level.size() - 1; remaining > 0; --remaining) {
        for (std::size_t i = 0; i < remaining; ++i) {
            level[i].x += t * (level[i + 1].x - level[i].x);
            level[i].y += t * (level[i + 1].y - level[i].y);
        }
    }
    return level.front();
}

}

double Lagr(CMatrix& X, CMatrix& Y, double x)
{
    if (X.rows() != Y.rows() || X.rows() == 0 || !std::isfinite(x)) {
        throw std::invalid_argument("Lagrange interpolation requires matching nonempty finite vectors");
    }
    const int count = X.rows();
    for (int i = 0; i < count; ++i) {
        if (!std::isfinite(X(i)) || !std::isfinite(Y(i))) {
            throw std::invalid_argument("interpolation data must be finite");
        }
        if (i > 0 && X(i) <= X(i - 1)) {
            throw std::invalid_argument("abscissas must be strictly increasing and unique");
        }
    }
    if (x < X(0) || x > X(count - 1)) {
        throw std::out_of_range("interpolation argument is outside the node interval");
    }
    for (int i = 0; i < count; ++i) {
        if (x == X(i)) {
            return Y(i);
        }
    }

    double result = 0.0;
    for (int i = 0; i < count; ++i) {
        double basis = 1.0;
        for (int j = 0; j < count; ++j) {
            if (i != j) {
                basis *= (x - X(j)) / (X(i) - X(j));
            }
        }
        result += Y(i) * basis;
    }
    return result;
}

std::vector<lab11::Point> lab11::AssignmentLagrangeNodes()
{
    constexpr double pi_value = 3.14159265358979323846;
    std::vector<Point> nodes;
    nodes.reserve(5);
    for (int i = 0; i <= 4; ++i) {
        const double x = static_cast<double>(i) * pi_value / 4.0;
        nodes.push_back({x, std::pow(2.0 + std::cos(x), std::sin(2.0 * x))});
    }
    return nodes;
}

std::vector<lab11::Point> lab11::AssignmentLagrangeCurve(const std::vector<Point>& nodes)
{
    if (nodes.size() < 2 || nodes.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::invalid_argument("Lagrange curve requires at least two nodes");
    }
    CMatrix x_nodes(static_cast<int>(nodes.size()));
    CMatrix y_nodes(static_cast<int>(nodes.size()));
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        x_nodes(static_cast<int>(i)) = nodes[i].x;
        y_nodes(static_cast<int>(i)) = nodes[i].y;
    }
    std::vector<Point> curve;
    curve.reserve(17);
    const double left = nodes.front().x;
    const double right = nodes.back().x;
    for (int i = 0;; ++i) {
        const double x = left + static_cast<double>(i) * 0.2;
        if (x >= right) {
            break;
        }
        curve.push_back({x, Lagr(x_nodes, y_nodes, x)});
    }
    curve.push_back({right, Lagr(x_nodes, y_nodes, right)});
    return curve;
}

std::vector<lab11::Point> lab11::BezierPoints(const std::vector<Point>& control_points,
                                             int segments)
{
    if (control_points.size() < 2 || segments <= 0 || segments == std::numeric_limits<int>::max()) {
        throw std::invalid_argument("Bezier requires at least two controls and a positive segment count");
    }
    for (const Point& point : control_points) {
        if (!std::isfinite(point.x) || !std::isfinite(point.y)) {
            throw std::invalid_argument("Bezier control points must be finite");
        }
    }
    std::vector<Point> curve;
    curve.reserve(static_cast<std::size_t>(segments) + 1);
    for (int i = 0; i <= segments; ++i) {
        const double t = static_cast<double>(i) / static_cast<double>(segments);
        curve.push_back(evaluate_bezier(control_points, t));
    }
    return curve;
}

void Bezier(CMatrix& X, CMatrix& Y, CMatrix& XB, CMatrix& YB, int M)
{
    validate_control_points(X, Y);
    if (M <= 0 || M == std::numeric_limits<int>::max()) {
        throw std::invalid_argument("Bezier segment count must be positive");
    }
    std::vector<lab11::Point> controls;
    controls.reserve(static_cast<std::size_t>(X.rows()));
    for (int i = 0; i < X.rows(); ++i) {
        controls.push_back({X(i), Y(i)});
    }
    const std::vector<lab11::Point> curve = lab11::BezierPoints(controls, M);
    CMatrix result_x(static_cast<int>(curve.size()));
    CMatrix result_y(static_cast<int>(curve.size()));
    for (std::size_t i = 0; i < curve.size(); ++i) {
        result_x(static_cast<int>(i)) = curve[i].x;
        result_y(static_cast<int>(i)) = curve[i].y;
    }
    XB = std::move(result_x);
    YB = std::move(result_y);
}

void lab11::WritePointFile(const std::string& path, const std::vector<Point>& points)
{
    if (path.empty()) {
        throw std::invalid_argument("output path must not be empty");
    }
    for (const Point& point : points) {
        if (!std::isfinite(point.x) || !std::isfinite(point.y)) {
            throw std::invalid_argument("exported coordinates must be finite");
        }
    }
    const std::filesystem::path output_path(path);
    if (output_path.has_parent_path()) {
        std::filesystem::create_directories(output_path.parent_path());
    }
    std::ofstream output(output_path);
    if (!output) {
        throw std::runtime_error("could not open coordinate output file");
    }
    output << std::setprecision(std::numeric_limits<double>::max_digits10);
    for (const Point& point : points) {
        output << point.x << '\t' << point.y << '\n';
    }
    if (!output) {
        throw std::runtime_error("could not write coordinate output file");
    }
}
