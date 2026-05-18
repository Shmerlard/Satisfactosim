#include "FactoryEdgeNode.h"
#include "Factory.h"
#include "FactoryNode.h"

FactoryEdgeNode::FactoryEdgeNode(
    Factory& parentFactory,
    PortType edgeType,
    QString name,
    QUuid id)
    : AbstractNode(parentFactory, name, id)
    , m_edgeType(edgeType)
{
    m_type = NodeType::FactoryEdge;
    auto& selectedList = (edgeType == PortType::Input) ? m_outputs : m_inputs;
    selectedList.push_back(std::make_unique<Port>(*this, nullptr, !edgeType));
    parentFactory.node()->addEdgePort(this);
}

Frac FactoryEdgeNode::portRate(const Port* port) const
{
    // FIX: implement
    return Frac(0);
}

void FactoryEdgeNode::onPortConnected(Port& port)
{
    // FIX: fix when there is more than one connection already
    this->m_mirrorPort->item = this->port()->item;
    emit itemChanged();
    emit portsChanged();
    emit m_mirrorPort->owner.portsChanged();
}

void FactoryEdgeNode::onPortDisconnected(Port& port)
{
    if (this->port()->connections.empty()) {
        this->port()->item = nullptr;
        this->port()->amount = 0;
        this->m_mirrorPort->item = nullptr;
        this->m_mirrorPort->amount = 0;
    }
    emit itemChanged();
    emit portsChanged();
    emit m_mirrorPort->owner.portsChanged();
}

QJsonObject FactoryEdgeNode::getJsonNode() const
{
    QJsonObject obj = AbstractNode::getJsonNode();
    // obj["edgeType"] = stringFromPortType(edgeType());
    obj["edgeType"] = static_cast<uint8_t>(edgeType());
    obj["portIndex"] = m_mirrorPort->owner.getPortIndex(*m_mirrorPort);
    return obj;
}


Port* FactoryEdgeNode::port() const
{
    auto& selectedList = (m_edgeType == PortType::Input) ? m_outputs : m_inputs;
    return selectedList.empty() ? nullptr : selectedList[0].get();
}


QString FactoryEdgeNode::itemName() const
{
    if (!port() || !port()->item)
        return "";
    return port()->item->itemName;
}
