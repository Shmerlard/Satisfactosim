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

float FactoryNode::portRate(const Port* port) const
{
    // FIX: implement
    return 0.f;
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

// bool FactoryNode::connectToPort(Port& src, Port& dst, QString* err)
// {
//     // FIX: need more work over edge cases
//     AbstractNode::connectToPort(src, dst, err);
//
//     Port* ownedNode = (&src.owner == this) ? &src : (&dst.owner == this) ? &dst
//                                                                          : nullptr;
//
//     if (!ownedNode)
//         return false;
//     FactoryEdgeNode* matching = m_portEdges.value(ownedNode);
//     matching->port()->item = ownedNode->item;
//     return true;
//     // FIX: what will happen if i connect 2 uninitialized factory nodes
// }
void FactoryNode::onPortConnected(Port& port)
{
    // FIX: need to make sure port is under "this"
    FactoryEdgeNode* matchingEdge = m_portEdges.value(&port);
    if (!matchingEdge->port()->item) {
        if (port.item)
            matchingEdge->port()->item = port.item;

    }
}

void FactoryNode::onPortDisconnected(Port& port)
{
    // FIX: also needs to check if the edge node is also empty
    // FIX: needs to check that theport is actually this=parent
    if (port.connections.empty()) {
        port.item = nullptr;
        m_portEdges.value(&port)->port()->item = nullptr;
    }
}

