#pragma once

// #include <QString>
#include <QList>

class AbstractNode;
struct Item;

enum class PortType {Input, Output};

struct Port {
    Item* item;
    PortType type;
    AbstractNode& owner;
    QList<Port*> connectedTo;
    float amount;

    void disconnect() {
        for (auto* peer : connectedTo) {
            peer->connectedTo.removeAll(this);
        }
        connectedTo.clear();
    }

    void disconnect(Port& peer) {
        peer.connectedTo.removeOne(this);
        connectedTo.removeOne(&peer);
    }

    void connect(Port& peer) {
        connectedTo.append(&peer);
        peer.connectedTo.append(this);
    }

    ~Port() {
        disconnect();
    }

    explicit Port(AbstractNode& owner, Item* item, PortType type)
        : item(item), type(type), owner(owner), amount(0.0f) {}
};

