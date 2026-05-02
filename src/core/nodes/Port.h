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

    explicit Port(AbstractNode& owner, Item* item, PortType type)
        : item(item), type(type), owner(owner), amount(0.0f) {}
};

