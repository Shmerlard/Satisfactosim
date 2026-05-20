#include "Factory.h"
#include "Connection.h"
#include "ExtractionNode.h"
#include "FactoryEdgeNode.h"
#include "FactoryNode.h"
#include "ProductionNode.h"
#include "SplitterNode.h"

Factory::Factory(Factory* parent, QString name, QUuid id)
    : QObject(parent)
    , m_id(id.isNull() ? QUuid::createUuid() : id)
    , m_parent(parent)
    , m_name(name)
    , m_node(nullptr)
{
}

SplitterNode* Factory::createSplitterNode(QList<Frac> weights, QString name)
{
    SplitterNode* ptr = new SplitterNode(*this, weights, name);
    addNode(*ptr);
    return ptr;
}

ProductionNode* Factory::createProductionNode(const ProductionRecipe& recipe, QString name)
{
    // FIX: add chehcks?
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

Factory* Factory::createFactory(QString name, bool ignoreNode)
{
    Factory* facPtr = new Factory(this, name, QUuid());

    if (!ignoreNode) {
        FactoryNode* facNodePtr = new FactoryNode(*this, *facPtr, name);
        addNode(*facNodePtr);
    }

    m_subFactories.append(facPtr);
    return facPtr;
}

FactoryEdgeNode* Factory::createFactoryEdgeNode(PortType edgeType, QString name, QString* err)
{
    if (!m_parent) {
        if (err)
            *err = "Can't add edge node in the root factory";
        return nullptr;
    }
    FactoryEdgeNode* edge = new FactoryEdgeNode(*this, edgeType, name);
    // m_edges.push_back(edge);
    addNode(*edge);
    return edge;
}

FactoryNode* Factory::createFactoryNode(Factory* targetFactory, QString name)
{
    FactoryNode* node = new FactoryNode(*this, *targetFactory, name);
    addNode(*node);
    return node;
}

Connection* Factory::connect(Port& a, Port& b, QString* err)
{
    QString _dummy;
    if (!err)
        err = &_dummy;
    if (&a.owner == &b.owner) {
        *err = "source and destination belong to the same node!";
        return nullptr;
    }
    if (a.type == b.type) {
        *err = "source and destination are the same type!";
        return nullptr;
    }
    if (b.isConnected(a)) {
        *err = "source and destination are already connected!";
        return nullptr;
    }
    if (!b.item && !a.item) {
        *err = "cannot connect 2 empty items";
        return nullptr;
    }

    if (!b.item)
        b.item = a.item;
    else if (!a.item)
        a.item = b.item;
    else if (b.item != a.item) {
        *err = "cannot connect two different items ports";
        return nullptr;
    }

    Connection* connection = new Connection(&a, &b);
    connection->setParent(this);
    m_connections.append(connection);

    a.connections.append(connection);
    b.connections.append(connection);
    a.owner.onPortConnected(a);
    b.owner.onPortConnected(b);
    err->clear();
    return connection;
}

void Factory::disconnect(Port& a, Port& b, QString* err)
{
    QString _dummy;
    if (!err)
        err = &_dummy;

    Connection* conn = a.connection(b);
    if (!conn) {
        *err = "Ports are not connected";
        return;
    }
    a.connections.removeOne(conn);
    b.connections.removeOne(conn);
    a.owner.onPortDisconnected(a);
    b.owner.onPortDisconnected(b);
    m_connections.removeOne(conn);
    delete conn;
}

void Factory::addNode(AbstractNode& node)
{
    // int inserIndex = m_nodes.count();
    // for (int i = 0; i < inserIndex; i++) {
    //     if (m_nodes[i]->type() > node.type()) {
    //         inserIndex = i;
    //         break;
    //     }
    // }
    // m_nodes.insert(inserIndex, &node);
    m_nodes.append(&node);

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
    auto disconnectPorts = [&](const std::vector<std::unique_ptr<Port>>& ports) {
        for (const auto& port : ports) {
            for (Connection* conn : QList<Connection*>(port->connections)) {
                Port* peer = conn->getPeer(*port);
                port->connections.removeOne(conn);
                peer->connections.removeOne(conn);
                peer->owner.onPortDisconnected(*peer);
                m_connections.removeOne(conn);
                delete conn;
            }
        }
    };
    disconnectPorts(node.inputs());
    disconnectPorts(node.outputs());

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
