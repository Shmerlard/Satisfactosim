#pragma once

#include "AbstractSolver.h"
#include "core/types/Types.h"
#include <QList>
#include <QMap>
#include <vector>

class MachineNode;
class Factory;

struct Equation {
    QMap<MachineNode*, Frac> coefficients;
    Frac rhs;

    int varCount() { return coefficients.size(); }
    bool contains(MachineNode* node) { return coefficients.contains(node); }
};

using Matrix = std::vector<std::vector<Frac>>;

class GaussianSolver : public AbstractSolver {
private:
    QList<MachineNode*> m_nodes;
    QList<Equation> m_equations;
    QMap<MachineNode*, QList<Equation*>> m_nodeEquations;

public:
    void reset() override;
    void build(Factory* root) override;
    void solve() override;

private:
    // Reduces mat to RREF in place. Returns pivot column index per row (-1 if no pivot).
    std::vector<int> gaussianEliminate(Matrix& mat, int numVars);
};
