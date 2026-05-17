#include "GaussianSolver.h"
#include <QDebug>
#include "src/core/nodes/AbstractNode.h"
#include "src/core/nodes/Connection.h"
#include "src/core/nodes/Factory.h"
#include "src/core/nodes/MachineNode.h"
#include <QQueue>
#include <QSet>

// --- row operations ---

static std::vector<Frac> operator*(const std::vector<Frac>& row, Frac scalar)
{
    std::vector<Frac> result = row;
    for (auto& v : result)
        v *= scalar;
    return result;
}

static std::vector<Frac> operator-(const std::vector<Frac>& a, const std::vector<Frac>& b)
{
    std::vector<Frac> result = a;
    for (int i = 0; i < (int)a.size(); ++i)
        result[i] -= b[i];
    return result;
}

// --- gaussian elimination ---

std::vector<int> GaussianSolver::gaussianEliminate(Matrix& mat, int numVars)
{
    int numRows = mat.size();
    int pivotRow = 0;
    std::vector<int> pivotColForRow(numRows, -1);

    for (int col = 0; col < numVars && pivotRow < numRows; ++col) {
        // find first nonzero in this column at or below pivotRow
        int found = -1;
        for (int row = pivotRow; row < numRows; ++row) {
            if (mat[row][col] != Frac(0)) { found = row; break; }
        }
        if (found == -1)
            continue;

        std::swap(mat[found], mat[pivotRow]);
        pivotColForRow[pivotRow] = col;

        // normalize pivot row so pivot = 1
        mat[pivotRow] = mat[pivotRow] * (Frac(1) / mat[pivotRow][col]);

        // eliminate this column in all other rows
        for (int row = 0; row < numRows; ++row) {
            if (row == pivotRow || mat[row][col] == Frac(0))
                continue;
            mat[row] = mat[row] - mat[pivotRow] * mat[row][col];
        }

        ++pivotRow;
    }

    return pivotColForRow;
}

// --- reset ---

void GaussianSolver::reset()
{
    m_nodes.clear();
    m_islands.clear();
    m_islandEquations.clear();
}

// --- build ---

void GaussianSolver::build(Factory* root)
{
    // --- collect all machine nodes ---
    for (auto* node : root->nodes())
        if (node->isMachineNode() && !m_nodes.contains(node))
            m_nodes.append(static_cast<MachineNode*>(node));

    // --- detect islands via BFS ---
    QSet<MachineNode*> visited;
    for (MachineNode* startNode : m_nodes) {
        if (visited.contains(startNode))
            continue;

        Island island;
        QQueue<MachineNode*> queue;
        queue.enqueue(startNode);
        visited.insert(startNode);

        while (!queue.isEmpty()) {
            MachineNode* current = queue.dequeue();
            island.append(current);

            auto visitPort = [&](const std::unique_ptr<Port>& uPort) {
                for (Connection* conn : uPort->connections) {
                    AbstractNode* peerNode = &conn->getPeer(*uPort)->owner;
                    if (!peerNode->isMachineNode())
                        continue;
                    auto* peer = static_cast<MachineNode*>(peerNode);
                    if (!visited.contains(peer) && m_nodes.contains(peer)) {
                        visited.insert(peer);
                        queue.enqueue(peer);
                    }
                }
            };

            for (auto& uPort : current->inputs())  visitPort(uPort);
            for (auto& uPort : current->outputs()) visitPort(uPort);
        }

        m_islands.append(island);

        // --- build equations for this island ---
        QList<Equation> equations;
        for (auto* node : island) {
            for (auto& uPort : node->inputs()) {
                Port* port = uPort.get();
                if (port->connections.isEmpty())
                    continue;
                Equation eq;
                eq.rhs = Frac(0);
                eq.coefficients[node] = -1 * node->portRate(port);
                for (Connection* conn : port->connections) {
                    Port* peer = conn->getPeer(*port);
                    Frac t = Frac(1, peer->connections.size()); // FIX: add override option
                    eq.coefficients[static_cast<MachineNode*>(&peer->owner)] = t * peer->owner.portRate(peer);
                }
                equations.append(eq);
            }
        }
        m_islandEquations.append(equations);
    }
}

// --- solve ---

void GaussianSolver::solve()
{
    if (m_islands.isEmpty())
        return;

    for (int islandIdx = 0; islandIdx < m_islands.size(); ++islandIdx) {
        const Island& island = m_islands[islandIdx];
        const QList<Equation>& islandEqs = m_islandEquations[islandIdx];

        int numVars = island.size();
        int numEqs  = islandEqs.size();
        if (numEqs == 0)
            continue;

        // --- build dense matrix [A | b] ---
        Matrix mat(numEqs, std::vector<Frac>(numVars + 1, Frac(0)));
        for (int i = 0; i < numEqs; ++i) {
            for (auto it = islandEqs[i].coefficients.begin(); it != islandEqs[i].coefficients.end(); ++it)
                mat[i][island.indexOf(it.key())] = it.value();
            mat[i][numVars] = islandEqs[i].rhs;
        }

        // --- gaussian eliminate ---
        std::vector<int> pivotColForRow = gaussianEliminate(mat, numVars);

        // --- identify pivot and free columns ---
        QSet<int> pivotCols;
        for (int r = 0; r < numEqs; ++r)
            if (pivotColForRow[r] != -1)
                pivotCols.insert(pivotColForRow[r]);

        int numFreeVars = numVars - (int)pivotCols.size();
        if (numFreeVars > 1)
            qWarning() << "Island has" << numFreeVars << "free variables — may be underdetermined";

        // set each free variable to 1
        QMap<MachineNode*, Frac> counts;
        for (int col = 0; col < numVars; ++col)
            if (!pivotCols.contains(col))
                counts[island[col]] = Frac(1);

        // back-substitute
        for (int r = 0; r < numEqs; ++r) {
            int pc = pivotColForRow[r];
            if (pc == -1)
                continue;
            Frac val = mat[r][numVars];
            for (int col = 0; col < numVars; ++col) {
                if (col == pc || mat[r][col] == Frac(0))
                    continue;
                if (counts.contains(island[col]))
                    val -= mat[r][col] * counts[island[col]];
            }
            counts[island[pc]] = val;
        }

        // --- scale to match limits ---
        Frac scale(1);
        bool hasLimit = false;
        for (auto* node : island) {
            if (node->machineLimit() < 0)
                continue;
            if (!counts.contains(node) || counts[node] == Frac(0))
                continue;
            Frac needed = Frac(node->machineLimit()) / counts[node];
            if (!hasLimit || needed < scale) {
                scale = needed;
                hasLimit = true;
            }
        }
        if (hasLimit)
            for (auto& count : counts)
                count *= scale;

        // --- apply to nodes ---
        for (auto it = counts.begin(); it != counts.end(); ++it) {
            MachineNode* node = it.key();
            Frac count = it.value();
            node->setMachineCount(count);
            for (auto& port : node->inputs())
                port->amount = boost::rational_cast<float>(count * node->portRate(port.get()));
            for (auto& port : node->outputs())
                port->amount = boost::rational_cast<float>(count * node->portRate(port.get()));
        }
    }
}
