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
    , m_machineCount(machineCount)
    , m_machineLimit(machineLimit)
{
}

QJsonObject MachineNode::getJsonNode() const
{
    QJsonObject obj = AbstractNode::getJsonNode();
    obj["machineLimit"] = m_machineLimit;
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
