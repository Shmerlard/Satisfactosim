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

float FactoryEdgeNode::portRate(const Port* port) const
{
    return 0.f;
}

void FactoryEdgeNode::onPortConnected(Port& port) 
{
    // FIX: fix when there i s more then one connection already
    // make sure its not in root factory
    // this->m_parentFactory->node()
    this->m_mirrorPort->item = this->port()->item;
}

void FactoryEdgeNode::onPortDisconnected(Port& port)
{
    // FIX: also needs to check if the edge node is also empty
    // FIX: needs to check that theport is actually this=parent
    if(this->port()->connectedTo.empty()) {
        this->port()->item = nullptr;
        this->m_mirrorPort->item = nullptr;

    }
}

QJsonObject FactoryEdgeNode::getJsonNode() const
{
    QJsonObject obj = AbstractNode::getJsonNode();
    // obj["edgeType"] = stringFromPortType(edgeType());
    obj["edgeType"] = stringFromPortType(edgeType());
    return obj;
}

// Needs a special care in order to set the item to null when it's empty
void FactoryEdgeNode::disconnectPort(Port* port, Port* peer, QString* err)
{
    // FIX: add error
    AbstractNode::disconnectPort(port, peer, err);
    if (port->connectedTo.empty()) {
        port->item = nullptr;
    }
}

bool FactoryEdgeNode::connectToPort(Port& src, Port& dst, QString* err) {
    return AbstractNode::connectToPort(src, dst, err);

    // update item
}

Port* FactoryEdgeNode::port() const
{
    auto& selectedList = (m_edgeType == PortType::Input) ? m_outputs : m_inputs;
    return selectedList.empty() ? nullptr : selectedList[0].get();
}
