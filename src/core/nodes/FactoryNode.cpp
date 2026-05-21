#include "FactoryNode.h"
#include "FactoryEdgeNode.h"

FactoryNode::FactoryNode(
    Factory& parentFactory,
    Factory& targetFactory,
    QString name)
    : AbstractNode(parentFactory, name)
    , m_factory(targetFactory)
{
    m_type = NodeType::Factory;
    targetFactory.m_node = this;
}

Frac FactoryNode::portRate(const Port* port) const
{
    // FIX: implement
    return Frac(0);
}


QString FactoryNode::getHeaderInfo() const
{
    return QString("┌─ [%1] Factory  \"%2\"  (%3 nodes inside)")
               .arg(index())
               .arg(m_name)
               .arg(m_factory.nodes().size());
}

QJsonObject FactoryNode::getJsonNode() const
{
    QJsonObject obj = AbstractNode::getJsonNode();
    obj["factoryId"] = m_factory.id().toString();
    return obj;
}

// Creates a Port for edge
Port* FactoryNode::addEdgePort(FactoryEdgeNode* edge)
{
    PortType newPortType = edge->edgeType();
    Item* edgeItem = edge->port()->item;
    Port* newPort = new Port(*this, edgeItem, newPortType);
    auto& selectedList = (newPortType == PortType::Input) ? m_inputs : m_outputs;
    selectedList.push_back(std::unique_ptr<Port>(newPort));
    m_portEdges.insert(newPort, edge);
    edge->setMirrorPort(*newPort);

    return newPort;
}

FactoryEdgeNode* FactoryNode::getEdgeNode(Port* port)
{
    return m_portEdges.value(port, nullptr);
}

void FactoryNode::onPortConnected(Port& port)
{
    FactoryEdgeNode* matchingEdge = m_portEdges.value(&port);
    if (!matchingEdge->port()->item) {
        if (port.item)
            matchingEdge->port()->item = port.item;
    }
    emit portsChanged();
    emit matchingEdge->itemChanged();
    emit matchingEdge->portsChanged();
}

void FactoryNode::onPortDisconnected(Port& port)
{
    if (port.connections.empty()) {
        port.item = nullptr;
        port.amount = 0;
        FactoryEdgeNode* edge = m_portEdges.value(&port);
        edge->port()->item = nullptr;
        edge->port()->amount = 0;
    }
    emit portsChanged();
    FactoryEdgeNode* matchingEdge = m_portEdges.value(&port);
    emit matchingEdge->itemChanged();
    emit matchingEdge->portsChanged();
}
