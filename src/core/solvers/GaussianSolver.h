#pragma once

#include "AbstractSolver.h"
#include "core/types/Types.h"
#include <QList>
#include <QMap>
#include <QQueue>
#include <QSet>
#include <vector>

class MachineNode;
class Factory;
class SessionManager;
class Connection;
class Port;

struct Equation {
    QMap<MachineNode*, Frac> coefficients;
    Frac rhs;

    int varCount() { return coefficients.size(); }
    bool contains(MachineNode* node) { return coefficients.contains(node); }
};

using Matrix = std::vector<std::vector<Frac>>;
using Island = QList<MachineNode*>;

class GaussianSolver : public AbstractSolver {
private:
    QList<MachineNode*> m_nodes;
    QList<Island> m_islands;
    QList<QList<Equation>> m_islandEquations;
    void collectMachineNodes(Factory* root);
    void handleConnection(Connection& connection, Port& port, Equation& eq);
    void visitPort(Port* port, QSet<MachineNode*>& visited, QQueue<MachineNode*>& queue);

public:
    explicit GaussianSolver(SessionManager* session);

    void onLoad() override;
    void clear() override;
    void reset() override;
    void build(Factory* root) override;
    void solve() override;
    QString report() const override;

private:
    // Reduces mat to RREF in place. Returns pivot column index per row (-1 if no pivot).
    std::vector<int> gaussianEliminate(Matrix& mat, int numVars);
};
