#pragma once

#include <vector>

class CMatrix {
public:
    CMatrix() = default;
    explicit CMatrix(int size);

    int rows() const noexcept;
    int cols() const noexcept;
    double& operator()(int index);
    double operator()(int index) const;
    void RedimMatrix(int size);

private:
    std::vector<double> values_;
};
