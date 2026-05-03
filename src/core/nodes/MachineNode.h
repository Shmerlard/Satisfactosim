#pragma once

#include "AbstractNode.h"
#include "Factory.h"

struct Machine;
// class Solver;

class MachineNode : public AbstractNode {
    friend class Solver;

protected:
    explicit MachineNode(
        Factory& parentFactory,
        QString name = QString(),
        float machineCount = 1,
        float machineLimit = -1);

    virtual void buildPortsFromRecipe() = 0;
    virtual void clearPorts() = 0; // FIX: move to abstract node
    Machine* m_machine;
    float m_machineCount;
    float m_machineLimit = -1;

public:
    float machineCount() const { return m_machineCount; }
    float machineLimit() const { return m_machineLimit; }
    void setMachineLimit(float limit) { m_machineLimit = limit; }
    virtual QString machineName() const = 0;
    QJsonObject getJsonNode() const override;
};
