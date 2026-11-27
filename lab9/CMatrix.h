#pragma once

#include <array>
#include <cstddef>

class CMatrix {
public:
    CMatrix() : values_{0.0, 0.0, 0.0} {}
    CMatrix(double first, double second, double third) : values_{first, second, third} {}
    double& operator()(std::size_t index) { return values_.at(index); }
    double operator()(std::size_t index) const { return values_.at(index); }
private:
    std::array<double, 3> values_;
};
