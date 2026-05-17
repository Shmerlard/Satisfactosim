#include "GaussianSolver.h"
#include "src/core/nodes/AbstractNode.h"
#include "src/core/nodes/Connection.h"
#include "src/core/nodes/Factory.h"
#include "src/core/nodes/MachineNode.h"
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
    m_equations.clear();
    m_nodeEquations.clear();
}

// --- build ---

void GaussianSolver::build(Factory* root)
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

// --- solve ---

void GaussianSolver::solve()
{
    int numVars = m_nodes.size();
    int numEqs  = m_equations.size();
    if (numVars == 0 || numEqs == 0)
        return;

    // --- build dense matrix [A | b] ---
    Matrix mat(numEqs, std::vector<Frac>(numVars + 1, Frac(0)));
    for (int i = 0; i < numEqs; ++i) {
        for (auto it = m_equations[i].coefficients.begin(); it != m_equations[i].coefficients.end(); ++it)
            mat[i][m_nodes.indexOf(it.key())] = it.value();
        mat[i][numVars] = m_equations[i].rhs;
    }

    // --- solve ---
    std::vector<int> pivotColForRow = gaussianEliminate(mat, numVars);

    // --- identify pivot and free columns ---
    QSet<int> pivotCols;
    for (int r = 0; r < numEqs; ++r)
        if (pivotColForRow[r] != -1)
            pivotCols.insert(pivotColForRow[r]);

    // set the first free variable to 1
    QMap<MachineNode*, Frac> counts;
    for (int col = 0; col < numVars; ++col) {
        if (!pivotCols.contains(col)) {
            counts[m_nodes[col]] = Frac(1);
            break;
        }
    }

    // back-substitute: each pivot row gives pivot_var = rhs - sum(free * coeff)
    for (int r = 0; r < numEqs; ++r) {
        int pc = pivotColForRow[r];
        if (pc == -1)
            continue;
        Frac val = mat[r][numVars];
        for (int col = 0; col < numVars; ++col) {
            if (col == pc || mat[r][col] == Frac(0))
                continue;
            if (counts.contains(m_nodes[col]))
                val -= mat[r][col] * counts[m_nodes[col]];
        }
        counts[m_nodes[pc]] = val;
    }

    // --- scale to match limits ---
    Frac scale(1);
    bool hasLimit = false;
    for (auto* node : m_nodes) {
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
