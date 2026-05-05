#include "FactoryEdgeNode.h"

FactoryEdgeNode::FactoryEdgeNode(
    Factory& parentFactory,
    PortType edgeType,
    QString name,
    QUuid id)
    : AbstractNode(parentFactory, name, id)
{
    m_type = NodeType::FactoryEdge;
    // possibly more
}
