#include "FactoryNode.h"

FactoryNode::FactoryNode(
    Factory& parentFactory,
    Factory& ownedFactory,
    QString name)
    : AbstractNode(parentFactory, name)
    , m_factory(ownedFactory)
{
    m_type = NodeType::Factory;
    ownedFactory.m_node = this;
    parentFactory.addNode(*this);
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
