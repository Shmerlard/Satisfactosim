#pragma once

#include <memory>

class Factory;

class AbstractSolver {
public:
    enum class SolverType { Gaussian };

    static std::unique_ptr<AbstractSolver> create(SolverType type);

    virtual void reset() = 0;
    virtual void build(Factory* root) = 0;
    virtual void solve() = 0;
    virtual ~AbstractSolver() = default;
};
