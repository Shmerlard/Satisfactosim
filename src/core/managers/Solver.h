#pragma once

#include <QList>
#include <QMap>
#include <vector>
#include "core/types/Types.h"
// #include <Eigen/Core>
// #include <Eigen/SparseCore>

class MachineNode;
class AbstractNode;
class Connection;
class Factory;

// using Island = QList<AbstractNode*>;

struct Equation {
    QMap<MachineNode*, Frac> coefficients;
    Frac rhs;

    int varCount() { return coefficients.size(); }
    bool contains(MachineNode* node) { return coefficients.contains(node); }
};

using Matrix = std::vector<std::vector<Frac>>;

class Solver {
private:
    QList<MachineNode*> m_nodes;
    QList<Equation> m_equations;
    QMap<MachineNode*, QList<Equation*>> m_nodeEquations;

public:
    void build(Factory* root);
    void solve();

private:
    // Reduces mat to RREF in place. Returns pivot column index per row (-1 if no pivot).
    std::vector<int> gaussianEliminate(Matrix& mat, int numVars);
    // // --- flattened graph ---
    // QList<AbstractNode*> m_nodes;
    //
    // // --- variable indexing (one per node) ---
    // QMap<AbstractNode*, int> m_nodeVar;
    //
    // // --- augmented matrix [A|b], one row per wire ---
    // std::vector<std::vector<double>> m_matrix;
    // int m_numVars = 0;
    //
    // // --- helpers ---
    // void flattenFactory(Factory* factory);
    // void collectConnections(Factory* factory, QList<Connection*>& out);
    // void buildEquations(const QList<Connection*>& connections);
};
