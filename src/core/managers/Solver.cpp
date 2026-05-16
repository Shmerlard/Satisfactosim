#include "Solver.h"
#include "src/core/nodes/AbstractNode.h"
#include "src/core/nodes/Connection.h"
#include "src/core/nodes/Factory.h"
#include "src/core/nodes/FactoryEdgeNode.h"
#include "src/core/nodes/FactoryNode.h"
#include "src/core/nodes/MachineNode.h"
#include <QQueue>
#include <QSet>
#include <cmath>

void Solver::build(Factory* root)
{
    QQueue<Factory*> factoryQueue;
    factoryQueue.enqueue(root);
    // m_numVars = 0;
    m_map.clear();
    m_numVars = 0;
    int mapIndex = 0;
    while (!factoryQueue.empty()) {
        Factory* f = factoryQueue.dequeue();
        for (auto* node : f->nodes()) {
            if (node->type() == NodeType::Extraction || node->type() == NodeType::Production) {
                // m_numVars++;
                m_map.insert(node, mapIndex);
                mapIndex++;
            }
        }
        for (auto* subFactory : f->subFactories()) {
            factoryQueue.enqueue(subFactory);
        }
    }
    m_numVars = mapIndex;

    factoryQueue.enqueue(root);
    while (!factoryQueue.empty()) {
        Factory* f = factoryQueue.dequeue();
        for (auto* conn : f->connections()) {
            
        }

        for (auto* subFactory : f->subFactories())
            factoryQueue.enqueue(subFactory);
    }
}

void Solver::solve()
{

}
// void Solver::build(Factory* root) {
//     m_nodes.clear();
//     m_nodeVar.clear();
//     m_matrix.clear();
//     m_numVars = 0;
//
//     flattenFactory(root);
//
//     QList<Connection*> connections;
//     collectConnections(root, connections);
//     buildEquations(connections);
// }
//
// void Solver::flattenFactory(Factory* factory) {
//     for (AbstractNode* node : factory->nodes()) {
//         if (node->type() == NodeType::Production ||
//             node->type() == NodeType::Extraction) {
//             m_nodeVar[node] = m_numVars++;
//             m_nodes.append(node);
//         }
//         // FactoryNode and FactoryEdgeNode are skipped —
//         // the solver treats the whole tree as one flat graph
//     }
//     for (Factory* sub : factory->subFactories()) {
//         flattenFactory(sub);
//     }
// }
//
// void Solver::collectConnections(Factory* factory, QList<Connection*>& out) {
//     out.append(factory->connections());
//     for (Factory* sub : factory->subFactories())
//         collectConnections(sub, out);
// }
//
// void Solver::buildEquations(const QList<Connection*>& connections) {
//     QMap<Port*, QList<Connection*>> portConns;
//     QSet<Port*> allPorts;
//     for (Connection* c : connections) {
//         portConns[c->srcPort()].append(c);
//         portConns[c->dstPort()].append(c);
//         allPorts.insert(c->srcPort());
//         allPorts.insert(c->dstPort());
//     }
//
//     QSet<Port*> visited;
//
//     for (Port* startPort : allPorts) {
//         if (visited.contains(startPort)) continue;
//
//         QList<Port*> wirePorts;
//         QQueue<Port*> queue;
//         queue.enqueue(startPort);
//
//         while (!queue.isEmpty()) {
//             Port* port = queue.dequeue();
//             if (visited.contains(port)) continue;
//             visited.insert(port);
//
//             AbstractNode* owner = &port->owner;
//             NodeType t = owner->type();
//
//             if (t == NodeType::Production || t == NodeType::Extraction) {
//                 wirePorts.append(port);
//             } else if (t == NodeType::Factory) {
//                 FactoryNode* fn = static_cast<FactoryNode*>(owner);
//                 FactoryEdgeNode* edge = fn->portEdges().value(port);
//                 if (edge)
//                     queue.enqueue(edge->port());
//             } else if (t == NodeType::FactoryEdge) {
//                 FactoryEdgeNode* fen = static_cast<FactoryEdgeNode*>(owner);
//                 if (Port* mirror = fen->mirrorPort())
//                     queue.enqueue(mirror);
//             }
//
//             for (Connection* c : portConns.value(port)) {
//                 Port* peer = c->getPeer(*port);
//                 if (peer && !visited.contains(peer))
//                     queue.enqueue(peer);
//             }
//         }
//
//         if (wirePorts.isEmpty()) continue;
//
//         std::vector<double> row(m_numVars + 1, 0.0);
//         for (Port* port : wirePorts) {
//             int idx = m_nodeVar[&port->owner];
//             double rate = port->owner.portRate(port);
//             row[idx] += (port->type == PortType::Output) ? rate : -rate;
//         }
//         m_matrix.push_back(row);
//     }
//
//     // limited nodes: x_i = machineLimit
//     for (AbstractNode* node : m_nodes) {
//         MachineNode* mn = static_cast<MachineNode*>(node);
//         if (mn->machineLimit() >= 0) {
//             std::vector<double> row(m_numVars + 1, 0.0);
//             row[m_nodeVar[node]] = 1.0;
//             row[m_numVars] = mn->machineLimit();
//             m_matrix.push_back(row);
//         }
//     }
// }
//
// void Solver::solve() {
//     int rows = (int)m_matrix.size();
//     int cols  = m_numVars + 1; // last col is rhs
//
//     std::vector<int> pivotOfCol(m_numVars, -1); // col → row that has its pivot
//     int pivotRow = 0;
//
//     for (int col = 0; col < m_numVars && pivotRow < rows; ++col) {
//         // partial pivot: find largest absolute value in this column
//         int maxRow = pivotRow;
//         for (int r = pivotRow + 1; r < rows; ++r)
//             if (std::abs(m_matrix[r][col]) > std::abs(m_matrix[maxRow][col]))
//                 maxRow = r;
//
//         if (std::abs(m_matrix[maxRow][col]) < 1e-10)
//             continue; // free variable, skip
//
//         std::swap(m_matrix[pivotRow], m_matrix[maxRow]);
//
//         double scale = m_matrix[pivotRow][col];
//         for (int j = 0; j < cols; ++j)
//             m_matrix[pivotRow][j] /= scale;
//
//         // eliminate this column in all other rows
//         for (int r = 0; r < rows; ++r) {
//             if (r == pivotRow) continue;
//             double factor = m_matrix[r][col];
//             if (std::abs(factor) < 1e-10) continue;
//             for (int j = 0; j < cols; ++j)
//                 m_matrix[r][j] -= factor * m_matrix[pivotRow][j];
//         }
//
//         pivotOfCol[col] = pivotRow++;
//     }
//
//     // read solution (free variables stay 0)
//     std::vector<double> solution(m_numVars, 0.0);
//     for (int col = 0; col < m_numVars; ++col)
//         if (pivotOfCol[col] >= 0)
//             solution[col] = m_matrix[pivotOfCol[col]][m_numVars];
//
//     // write back machine counts and port amounts
//     for (AbstractNode* node : m_nodes) {
//         MachineNode* mn = static_cast<MachineNode*>(node);
//         double x = solution[m_nodeVar[node]];
//         mn->m_machineCount = static_cast<float>(x);
//
//         for (auto& port : node->inputs())
//             port->amount = static_cast<float>(x * node->portRate(port.get()));
//         for (auto& port : node->outputs())
//             port->amount = static_cast<float>(x * node->portRate(port.get()));
//     }
// }
