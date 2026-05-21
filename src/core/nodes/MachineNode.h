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
    Q_PROPERTY(int somersloopCount READ somersloopCount NOTIFY recipeChanged)
    Q_PROPERTY(int somersloopSlotSize READ somersloopSlotSize NOTIFY recipeChanged)
    Q_PROPERTY(float overclock READ overclockFloat WRITE setOverclockFloat NOTIFY recipeChanged)

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
    Frac m_overclock = Frac(1);
    int m_somersloopCount = 0;

    virtual void buildPortsFromRecipe() = 0;
    virtual Frac basePortRate(const Port* port) const = 0;
    virtual const Recipe* recipe() const { return m_recipe; }

public:
    QString machineName() const;
    int somersloopCount() const { return m_somersloopCount; }
    int somersloopSlotSize() const { return m_machine->somersloopSlotSize; }
    Frac machineCount() const { return m_machineCount; }
    Frac machineLimit() const { return m_machineLimit; }
    Frac overclock() const { return m_overclock; }
    float machineCountFloat() const { return boost::rational_cast<float>(m_machineCount); }
    float machineLimitFloat() const { return boost::rational_cast<float>(m_machineLimit); }
    float overclockFloat() const { return boost::rational_cast<float>(m_overclock); }
    QString machineCountStr() const;
    QString machineLimitStr() const;
    void setSomersloopCount(int count);
    Frac somersloopFactor() const;
    void setOverclock(Frac overclock);
    void setOverclockFloat(float overclock) { setOverclock(Frac(static_cast<int>(overclock * 100), 100)); }
    virtual QString machineIcon() const;
    void setMachineLimit(float limit);
    void setMachineLimit(Frac limit);

    Frac portRate(const Port* port) const override;
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
