#include "MachineNode.h"
#include "core/types/Types.h"
#include <QJsonObject>

MachineNode::MachineNode(
    Factory& parentFactory,
    QString name,
    float machineCount,
    float machineLimit)
    : AbstractNode(parentFactory, name)
    , m_machineCount(machineCount)
    , m_machineLimit(machineLimit)
{
}


QJsonObject MachineNode::getJsonNode() const
{
    QJsonObject obj;
    obj["id"]           = m_id.toString();
    obj["name"]         = m_name;
    obj["machineLimit"] = m_machineLimit;
    return obj;
}



