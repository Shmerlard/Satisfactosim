#include "MachineNode.h"
#include "core/types/Types.h"
#include <QJsonObject>

MachineNode::MachineNode(
    Factory& parentFactory,
    QString name,
    float machineCount,
    float machineLimit,
    QUuid id)
    : AbstractNode(parentFactory, name, id)
{
    m_machineLimit = Frac((int)(machineLimit * 100), 100);
    m_machineCount = Frac((int)(machineCount * 100), 100);
}

QJsonObject MachineNode::getJsonNode() const
{
    QJsonObject obj = AbstractNode::getJsonNode();
    obj["machineLimit"] = boost::rational_cast<double>(m_machineLimit);
    obj["recipe"] = recipe() ? recipe()->recipeClass : "";
    return obj;
}

QString MachineNode::machineIcon() const
{
    if (!m_machine)
        return QString();
    return "image://assets/machine/" + m_machine->machineClass;
}

QString MachineNode::machineName() const
{
    if (!m_machine)
        return QString();
    return m_machine->machineName;
}

QString MachineNode::machineCountStr() const
{
    QString str;
    str = StringFromFrac(m_machineCount);
    return str;
}

QString MachineNode::machineLimitStr() const
{
    QString str;
    str = StringFromFrac(m_machineLimit);
    return str;
}

void MachineNode::setSomersloopCount(int count)
{
    if (count < 0 || count > m_machine->somersloopSlotSize)
        return;
    m_somersloopCount = count;
    emit recipeChanged();
}

void MachineNode::setOverclock(Frac overclock)
{
    overclock = std::max(Frac(1, 100), std::min(Frac(5, 2), overclock));
    m_overclock = overclock;
    emit recipeChanged();
}

void MachineNode::setMachineLimit(float limit)
{
    m_machineLimit = Frac((int)(limit * 100), 100);
    emit machineLimitChanged();
}

void MachineNode::setMachineLimit(Frac limit)
{
    m_machineLimit = limit;
    emit machineLimitChanged();
}
