#include "AbstractSolver.h"
#include "GaussianSolver.h"

AbstractSolver::AbstractSolver(SessionManager* session, QObject* parent)
    : QObject(parent)
    , m_session(session)
{
}

AbstractSolver* AbstractSolver::create(SolverType type, SessionManager* session)
{
    switch (type) {
    case SolverType::Gaussian:
        return new GaussianSolver(session);
    }
    return nullptr;
}
