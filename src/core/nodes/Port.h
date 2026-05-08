#pragma once

// #include <QString>
#include <QList>

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
    QList<Port*> connectedTo;
    float amount;

    void disconnect()
    {
        for (auto* peer : connectedTo) {
            peer->connectedTo.removeAll(this);
        }
        connectedTo.clear();
    }

    void disconnect(Port& peer)
    {
        peer.connectedTo.removeOne(this);
        connectedTo.removeOne(&peer);
    }

    void connect(Port& peer)
    {
        connectedTo.append(&peer);
        peer.connectedTo.append(this);
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
