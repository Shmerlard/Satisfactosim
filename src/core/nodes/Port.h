#pragma once

#include <QList>
#include "Connection.h"
#include <QPointF>

class AbstractNode;
class FactoryEdgeNode;
struct Item;

enum class PortType : uint8_t {
    Input,
    Output
};

inline PortType portTypeFromString(QString str)
{
    if (str == "input" || str == "in")
        return PortType::Input;
    return PortType::Output;
}

inline QString stringFromPortType(PortType type)
{
    if (type == PortType::Input)
        return "input";
    return "output";
}

inline PortType operator!(PortType t)
{
    return t == PortType::Input ? PortType::Output : PortType::Input;
}

struct Port {
    Item* item;
    PortType type;
    AbstractNode& owner;
    QList<Connection*> connections;
    float amount; // the sum of amounts from all connections
    QPointF offset; // the offset from the node pos to the connection point pos

    void disconnect()
    {
        connections.clear();
    }

    bool isConnected(Port& peer) {
        for (auto* conn : connections) {
            if (conn->getPeer(*this) == &peer)
                return true;
        }
        return false;
    }

    Connection* connection(Port& peer) {
        for (auto* conn : connections) {
            if (conn->getPeer(*this) == &peer)
                return conn;
        }
        return nullptr;
    }

    ~Port() { disconnect(); }

    explicit Port(AbstractNode& owner, Item* item, PortType type)
        : item(item)
        , type(type)
        , owner(owner)
        , amount(0.0f)
    {
    }
};
