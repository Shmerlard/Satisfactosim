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

class Solver {
private:
    // QList<Island> m_map;
    QList<MachineNode*> m_nodes;
    QList<Equation> m_equations;
    QMap<MachineNode*, QList<Equation*>> m_nodeEquations;

public:
    void build(Factory* root);
    void solve();

private:
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
