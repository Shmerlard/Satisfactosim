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
    Q_PROPERTY(float machineCount READ machineCount) // FIX: add notify
    Q_PROPERTY(float machineLimit READ machineLimit) // FIX: add notify

    friend class Solver;

protected:
    explicit MachineNode(
        Factory& parentFactory,
        QString name = QString(),
        float machineCount = 1,
        float machineLimit = -1,
        QUuid id = QUuid());

    const Machine* m_machine = nullptr;
    const Recipe* m_recipe = nullptr;
    float m_machineCount = -1;
    float m_machineLimit = -1;

    virtual void buildPortsFromRecipe() = 0;
    virtual const Recipe* recipe() const { return m_recipe; }

public:
    QString machineName() const;
    float machineCount() const { return m_machineCount; }
    float machineLimit() const { return m_machineLimit; }
    QString machineIcon() const;
    void setMachineLimit(float limit) { m_machineLimit = limit; }
    QJsonObject getJsonNode() const override;

signals:
    void recipeChanged();
};
