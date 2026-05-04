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
    // TODO: m_machine and m_recipe are never initialized here — both are garbage pointers until set by subclass
}


QJsonObject MachineNode::getJsonNode() const
{
    QJsonObject obj;
    obj["id"]           = m_id.toString();
    obj["name"]         = m_name;
    obj["machineLimit"] = m_machineLimit;
    return obj;
}



