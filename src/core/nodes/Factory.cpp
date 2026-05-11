#include "Factory.h"
#include "Connection.h"
#include "ExtractionNode.h"
#include "FactoryEdgeNode.h"
#include "FactoryNode.h"
#include "ProductionNode.h"

Factory::Factory(Factory* parent, QString name, QUuid id)
    : QObject(parent)
    , m_id(id.isNull() ? QUuid::createUuid() : id)
    , m_parent(parent)
    , m_name(name)
    , m_node(nullptr)
{
}

ProductionNode* Factory::createProductionNode(const ProductionRecipe& recipe, QString name)
{
    ProductionNode* ptr = new ProductionNode(*this, recipe, name);
    addNode(*ptr);
    return ptr;
}

ExtractionNode* Factory::createExtractionNode(const ExtractionRecipe& recipe, int tier, QString name)
{
    ExtractionNode* ptr = new ExtractionNode(*this, recipe, tier, name);
    addNode(*ptr);
    return ptr;
}

Factory* Factory::createFactory(QString name)
{
    Factory* facPtr = new Factory(this, name, QUuid());
    // facPtr->setParent(this);
    FactoryNode* facNodePtr = new FactoryNode(*this, *facPtr, name);
    addNode(*facNodePtr);

    m_subFactories.append(facPtr);
    return facPtr;
}

FactoryEdgeNode* Factory::createFactoryEdgeNode(PortType edgeType, QString name)
{
    if (!m_parent) {
        return nullptr;
        // FIX:ADD ERROR MESSAGE
    }
    FactoryEdgeNode* edge = new FactoryEdgeNode(*this, edgeType, name);
    // m_edges.push_back(edge);
    addNode(*edge);
    return edge;
}

Connection* Factory::connect(Port& a, Port& b)
{
    // if (a.type == b.type)
    //     return nullptr;
    // if (&a.owner == &b.owner)
    //     return nullptr;
    // if (a.owner.parentFactory() != this || b.owner.parentFactory() != this)
    //     return nullptr;
    // if (a.isConnected(b))
    //     return nullptr;
    //
    // Connection* connection = new Connection(&a, &b);
    // m_connections.push_back(std::unique_ptr<Connection>(connection));
    // a.connections.append(connection);
    // b.connections.append(connection);
    // return connection;
    Connection* conn = a.owner.connectToPort(a, b);
    if (conn) {
        conn->setParent(this);
        m_connections.append(conn);
    }
    return conn;
}

void Factory::disconnect(Port& a, Port& b)
{
    Connection* conn = a.owner.disconnectPort(a, b);
    if (conn) {
        m_connections.removeOne(conn);
        delete conn;
    }
}


void Factory::addNode(AbstractNode& node)
{
    int inserIndex = m_nodes.count();
    for (int i = 0; i < inserIndex; i++) {
        if (m_nodes[i]->type() > node.type()) {
            inserIndex = i;
            break;
        }
    }
    m_nodes.insert(inserIndex, &node);

    switch (node.type()) {
    case NodeType::FactoryEdge: {
        m_edges.append(static_cast<FactoryEdgeNode*>(&node));
        break;
    }
    default:
        break;
    }
}

void Factory::removeNode(AbstractNode& node)
{
    m_nodes.removeOne(&node);

    switch (node.type()) {
    case NodeType::FactoryEdge: {
        m_edges.removeOne(&node);
        break;
    }
    default:
        break;
    }
}
