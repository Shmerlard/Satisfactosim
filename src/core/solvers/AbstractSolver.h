#pragma once

#include <QObject>

class Factory;
class SessionManager;

class AbstractSolver : public QObject {
    Q_OBJECT;

protected:
    SessionManager* m_session;
public:
    enum class SolverType { Gaussian };

    static AbstractSolver* create(SolverType type, SessionManager* session);

    virtual void onLoad() { }
    virtual void clear() = 0;
    virtual void reset() = 0;
    virtual void build(Factory* root) = 0;
    virtual void solve() = 0;

    explicit AbstractSolver(SessionManager* session, QObject* parent = nullptr);
};
