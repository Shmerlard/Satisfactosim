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
    for (auto* node : root->nodes()) {
        if (node->type() == NodeType::Extraction || node->type() == NodeType::Production) {
            if (!m_nodes.contains(node))
                m_nodes.append(static_cast<MachineNode*>(node));

            for (auto& uPort : node->inputs()) {
                Port* port = uPort.get();

                if (port->connections.size() == 0)
                    continue;
                Equation eq;
                eq.rhs = Frac(0);
                eq.coefficients[static_cast<MachineNode*>(node)] = -1 * node->portRate(port);
                for (Connection* conn : port->connections) {
                    Port* peer = conn->getPeer(*port);
                    Frac t = Frac(1, peer->connections.size()); // FIX: add override option
                    eq.coefficients[static_cast<MachineNode*>(&peer->owner)] = t * peer->owner.portRate(peer);
                }

                m_equations.append(eq);
            }
        }
    }
    for (Equation& eq : m_equations)
        for (MachineNode* node : eq.coefficients.keys())
            m_nodeEquations[node].append(&eq);
}

void Solver::solve()
{
    QMap<MachineNode*, Frac> limits;
    for (auto* node : m_nodes)
        if (node->machineLimit() >= 0)
            limits.insert(node, node->machineLimit());
    if (limits.isEmpty())
        return;

    QMap<MachineNode*, Frac> counts;
    MachineNode* startNode = limits.firstKey();
    bool restarted = true;

    while (restarted) {
        restarted = false;
        counts.clear();

        QList<Equation> equations = m_equations;

        QMap<MachineNode*, QList<int>> nodeEqIdx;
        for (int i = 0; i < equations.size(); ++i)
            for (MachineNode* node : equations[i].coefficients.keys())
                nodeEqIdx[node].append(i);

        QQueue<MachineNode*> queue;
        counts[startNode] = limits[startNode];
        queue.enqueue(startNode);

        while (!queue.isEmpty() && !restarted) {
            MachineNode* current = queue.dequeue();
            Frac currentCount = counts[current];

            for (int idx : nodeEqIdx[current]) {
                Equation& eq = equations[idx];
                if (!eq.coefficients.contains(current))
                    continue;
                eq.rhs -= eq.coefficients[current] * currentCount;
                eq.coefficients.remove(current);

                if (eq.coefficients.isEmpty())
                    continue; // redundant or contradiction

                if (eq.coefficients.size() == 1) {
                    MachineNode* other = eq.coefficients.firstKey();
                    if (counts.contains(other))
                        continue;

                    Frac derived = eq.rhs / eq.coefficients[other];

                    if (limits.contains(other) && derived > limits[other]) {
                        startNode = other;
                        restarted = true;
                        break;
                    }

                    counts[other] = derived;
                    queue.enqueue(other);
                }
                // eq
                //
                // MachineNode* other = nullptr;
                // for (auto* n : eq->coefficients.keys()) {
                //     if (n != current) {
                //         other = n;
                //         break;
                //     }
                // }
                // if (!other || counts.contains(other))
                //     continue;
                // Frac derived = -(eq->coefficients[current] / eq->coefficients[other]) * currentCount;
                // if(limits.contains(other) && derived > limits[other]) {
                //     startNode = other;
                //     restarted = true;
                //     break;
                // }
                //
                // counts[other] = derived;
                // queue.enqueue(other);
            }
        }
    }

    for (auto it = counts.begin(); it != counts.end(); ++it) {
        MachineNode* node = it.key();
        Frac count = it.value();
        node->m_machineCount = count;
        for (auto& port : node->inputs())
            port->amount = boost::rational_cast<float>(count * node->portRate(port.get()));
        for (auto& port : node->outputs())
            port->amount = boost::rational_cast<float>(count * node->portRate(port.get()));
    }
}
