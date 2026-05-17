#include "AbstractSolver.h"
#include "GaussianSolver.h"

std::unique_ptr<AbstractSolver> AbstractSolver::create(SolverType type)
{
    switch (type) {
    case SolverType::Gaussian: return std::make_unique<GaussianSolver>();
    }
    return nullptr;
}
