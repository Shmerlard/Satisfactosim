#include "FactoryEdgeNode.h"
#include "Factory.h"

FactoryEdgeNode::FactoryEdgeNode(
    Factory& parentFactory,
    PortType edgeType,
    QString name,
    QUuid id)
    : AbstractNode(parentFactory, name, id)
{
    m_type = NodeType::FactoryEdge;
    // m_parentFactory->addNode(*this);
    // m_parentFactory->
    buildPort();
    // possibly more
}

void FactoryEdgeNode::buildPort()
{
    if (m_edgeType == PortType::Input)
        m_outputs.push_back(std::make_unique<Port>(*this, nullptr, PortType::Output));
    else
        m_inputs.push_back(std::make_unique<Port>(*this, nullptr, PortType::Input));

    // RESERVE 0 for the other
}

float FactoryEdgeNode::portRate(const Port* port) const
{
    return 0.f;
}

QJsonObject FactoryEdgeNode::getJsonNode() const
{
    return QJsonObject();
}
