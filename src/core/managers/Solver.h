#pragma once

#include <QList>
#include <QMap>
#include <vector>
// #include <Eigen/Core>
// #include <Eigen/SparseCore>

class AbstractNode;
class Connection;
class Factory;

class Solver {
private:
    QMap<AbstractNode*, int> m_map;
    int m_numVars;
    // Eigen::SparseMatrix<double> m_matrix;
    // Eigen::VectorXd m_b;

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
