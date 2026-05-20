#pragma once

#include "AbstractNode.h"
#include "Factory.h"

struct Machine;
struct Recipe;
// class Solver;

class MachineNode : public AbstractNode {
    Q_OBJECT;

    Q_PROPERTY(QString machineName READ machineName NOTIFY recipeChanged)
    Q_PROPERTY(QString machineIcon READ machineIcon NOTIFY recipeChanged)
    Q_PROPERTY(float machineCount READ machineCountFloat NOTIFY machineCountChanged)
    Q_PROPERTY(float machineLimit READ machineLimitFloat NOTIFY machineLimitChanged)
    Q_PROPERTY(QString machineCountStr READ machineCountStr NOTIFY machineCountChanged)
    Q_PROPERTY(QString machineLimitStr READ machineLimitStr NOTIFY machineLimitChanged)

    friend class GaussianSolver;

protected:
    explicit MachineNode(
        Factory& parentFactory,
        QString name = QString(),
        float machineCount = 1,
        float machineLimit = -1,
        QUuid id = QUuid());

    const Machine* m_machine = nullptr;
    const Recipe* m_recipe = nullptr;
    Frac m_machineCount = -1;
    Frac m_machineLimit = -1;

    virtual void buildPortsFromRecipe() = 0;
    virtual const Recipe* recipe() const { return m_recipe; }

public:
    QString machineName() const;
    Frac machineCount() const { return m_machineCount; }
    Frac machineLimit() const { return m_machineLimit; }
    float machineCountFloat() const { return boost::rational_cast<float>(m_machineCount); }
    float machineLimitFloat() const { return boost::rational_cast<float>(m_machineLimit); }
    QString machineCountStr() const;
    QString machineLimitStr() const;
    virtual QString machineIcon() const;
    void setMachineLimit(float limit);
    void setMachineLimit(Frac limit);
    QJsonObject getJsonNode() const override;
    void notifySolved() override
    {
        emit portsChanged();
        emit machineCountChanged();
    }

private:
    void setMachineCount(Frac count) { m_machineCount = count; } // only the solver can set the count

signals:
    void recipeChanged();
    void machineLimitChanged();
    void machineCountChanged();
};
