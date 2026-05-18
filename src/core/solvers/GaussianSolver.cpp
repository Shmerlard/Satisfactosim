#include "GaussianSolver.h"
#include "src/core/managers/SessionManager.h"
#include "src/core/nodes/AbstractNode.h"
#include "src/core/nodes/Connection.h"
#include "src/core/nodes/Factory.h"
#include "src/core/nodes/FactoryEdgeNode.h"
#include "src/core/nodes/FactoryNode.h"
#include "src/core/nodes/MachineNode.h"
#include <QDebug>
#include <QQueue>
#include <QSet>

GaussianSolver::GaussianSolver(SessionManager* session)
    : AbstractSolver(session, session)
{
}

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
            if (mat[row][col] != Frac(0)) {
                found = row;
                break;
            }
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

// --- onLoad / reset ---

void GaussianSolver::onLoad()
{
    connect(m_session, &SessionManager::machineLimitChanged, this, [this]() {
        solve();
        m_session->notifySolved();
    });
}

void GaussianSolver::clear()
{
    m_nodes.clear();
    m_islands.clear();
    m_islandEquations.clear();
}

void GaussianSolver::reset()
{
    disconnect(m_session, nullptr, this, nullptr);
    clear();
}

// --- build ---
void GaussianSolver::collectMachineNodes(QQueue<Factory*>& factoryQueue)
{

    while (!factoryQueue.isEmpty()) {
        Factory* f = factoryQueue.first();
        factoryQueue.dequeue();

        for (auto* node : f->nodes())
            if (node->isMachineNode() && !m_nodes.contains(node))
                m_nodes.append(static_cast<MachineNode*>(node));

        for (auto* sub : f->subFactories())
            factoryQueue.enqueue(sub);
    }
}

void GaussianSolver::handleConnection(Connection& conn, Port& port, Equation& eq)
{
    // FIX: CURRENTLY ONLY SUPPORT ONE INPUT each time
    Port* cPeer = nullptr;
    Port* cPort = &port;
    Connection* cConn = &conn;
    Frac t = Frac(-1);
    bool isMachineFound = false;
    while (!isMachineFound) {
        cPeer = cConn->getPeer(*cPort);
        AbstractNode* owner = &cPeer->owner;

        switch (owner->type()) {
        case NodeType::Factory: {
            FactoryNode* f = static_cast<FactoryNode*>(owner);
            FactoryEdgeNode* e = f->getEdgeNode(cPeer);
            Port* edgePort = e->port();
            if (!edgePort) {
                isMachineFound = true;
                break;
            }
            if (edgePort->connections.isEmpty()) {
                isMachineFound = true;
                break;
            }

            cPort = edgePort;
            cConn = edgePort->connections[0];
            break;
        }

        case NodeType::FactoryEdge: {
            FactoryEdgeNode* e = static_cast<FactoryEdgeNode*>(owner);
            Port* mirror = e->mirrorPort();
            if (!mirror || mirror->connections.isEmpty()) {
                isMachineFound = true;
                break;
            }
            cPort = mirror;
            cConn = mirror->connections[0];
            // isMachineFound = true;
            break;
        }
        case NodeType::Extraction:
        case NodeType::Production: {
            MachineNode* m = static_cast<MachineNode*>(&cPeer->owner);
            t = Frac(1, cPeer->connections.size());
            eq.coefficients[m] = t * cPeer->owner.portRate(cPeer);
            isMachineFound = true;
            break;
        }
        default:
            isMachineFound = true;
            break;
        }
    }
}

void GaussianSolver::visitPort(Port* port, QSet<MachineNode*>& visited, QQueue<MachineNode*>& queue)
{
    for (Connection* conn : port->connections) {
        Port* cPort = port;
        Connection* cConn = conn;
        bool done = false;

        while (!done) {
            Port* cPeer = cConn->getPeer(*cPort);
            AbstractNode* owner = &cPeer->owner;

            switch (owner->type()) {
            case NodeType::Factory: {
                FactoryNode* f = static_cast<FactoryNode*>(owner);
                FactoryEdgeNode* e = f->getEdgeNode(cPeer);
                Port* edgePort = e ? e->port() : nullptr;
                if (!edgePort || edgePort->connections.isEmpty()) {
                    done = true;
                    break;
                }
                cPort = edgePort;
                cConn = edgePort->connections[0];
                break;
            }
            case NodeType::FactoryEdge: {
                FactoryEdgeNode* e = static_cast<FactoryEdgeNode*>(owner);
                Port* mirror = e->mirrorPort();
                if (!mirror || mirror->connections.isEmpty()) {
                    done = true;
                    break;
                }
                cPort = mirror;
                cConn = mirror->connections[0];
                break;
            }
            case NodeType::Extraction:
            case NodeType::Production: {
                MachineNode* peer = static_cast<MachineNode*>(owner);
                if (!visited.contains(peer) && m_nodes.contains(peer)) {
                    visited.insert(peer);
                    queue.enqueue(peer);
                }
                done = true;
                break;
            }
            default:
                done = true;
                break;
            }
        }
    }
}

void GaussianSolver::build(Factory* root)
{
    QQueue<Factory*> factoryQueue;
    factoryQueue.enqueue(root);
    collectMachineNodes(factoryQueue);

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

            for (auto& uPort : current->inputs())
                visitPort(uPort.get(), visited, queue);
            for (auto& uPort : current->outputs())
                visitPort(uPort.get(), visited, queue);
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
                for (Connection* conn : port->connections)
                    handleConnection(*conn, *port, eq);

                if (eq.coefficients.size() > 1)
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
        int numEqs = islandEqs.size();

        // --- identify pivot columns (all free if no equations) ---
        QSet<int> pivotCols;

        // set each free variable to 1
        QMap<MachineNode*, Frac> counts;
        for (int col = 0; col < numVars; ++col)
            counts[island[col]] = Frac(1);

        if (numEqs > 0) {
            // --- build dense matrix [A | b] ---
            Matrix mat(numEqs, std::vector<Frac>(numVars + 1, Frac(0)));
            for (int i = 0; i < numEqs; ++i) {
                for (auto it = islandEqs[i].coefficients.begin(); it != islandEqs[i].coefficients.end(); ++it)
                    mat[i][island.indexOf(it.key())] = it.value();
                mat[i][numVars] = islandEqs[i].rhs;
            }

            // --- gaussian eliminate ---
            std::vector<int> pivotColForRow = gaussianEliminate(mat, numVars);

            for (int r = 0; r < numEqs; ++r)
                if (pivotColForRow[r] != -1)
                    pivotCols.insert(pivotColForRow[r]);

            int numFreeVars = numVars - (int)pivotCols.size();
            if (numFreeVars > 1)
                qWarning() << "Island has" << numFreeVars << "free variables — may be underdetermined";

            // back-substitute pivot variables
            for (int r = 0; r < numEqs; ++r) {
                int pc = pivotColForRow[r];
                if (pc == -1)
                    continue;
                counts.remove(island[pc]);  // pivot vars start unset
                Frac val = mat[r][numVars];
                for (int col = 0; col < numVars; ++col) {
                    if (col == pc || mat[r][col] == Frac(0))
                        continue;
                    if (counts.contains(island[col]))
                        val -= mat[r][col] * counts[island[col]];
                }
                counts[island[pc]] = val;
            }
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

        // --- propagate amounts to boundary node ports ---
        for (auto it = counts.begin(); it != counts.end(); ++it) {
            MachineNode* node = it.key();
            auto propagate = [](Port* machinePort) {
                for (Connection* conn : machinePort->connections) {
                    Port* peer = conn->getPeer(*machinePort);
                    if (peer->owner.isMachineNode())
                        continue;
                    peer->amount = machinePort->amount;
                    if (peer->owner.type() == NodeType::FactoryEdge) {
                        FactoryEdgeNode* edge = static_cast<FactoryEdgeNode*>(&peer->owner);
                        if (edge->mirrorPort())
                            edge->mirrorPort()->amount = machinePort->amount;
                    } else if (peer->owner.type() == NodeType::Factory) {
                        FactoryNode* fn = static_cast<FactoryNode*>(&peer->owner);
                        FactoryEdgeNode* edge = fn->getEdgeNode(peer);
                        if (edge && edge->port())
                            edge->port()->amount = machinePort->amount;
                    }
                }
            };
            for (auto& port : node->inputs())  propagate(port.get());
            for (auto& port : node->outputs()) propagate(port.get());
        }
    }
}
