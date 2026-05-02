#include "FactoryNode.h"

FactoryNode::FactoryNode(Factory& parentFactory, Factory& ownedFactory, QString name)
    : AbstractNode(parentFactory, name)
    , m_factory(ownedFactory)
{
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
    QJsonObject obj;
    obj["id"]        = m_id.toString();
    obj["type"]      = "factory";
    obj["name"]      = m_name;
    // obj["factoryId"] = m_factory.node()->id().toString();
    obj["factoryId"] = this->id().toString();
    return obj;
}
