#pragma once

#include "Port.h"
#include <QList>
#include <QString>

class AbstractNode;
class FactoryNode;

class Factory {
private:
    // friend class SessionManager;
    friend class FactoryNode;

    QString m_name;
    Factory* m_parent;
    FactoryNode* m_node;
    QList<AbstractNode*> m_subNodes;
    QList<Factory*> m_subFactories;
    // QList<Port*> m_inputs;
    // QList<Port*> m_outputs;

public:
    explicit Factory(Factory* parent, QString name);
    ~Factory(); // FIXME: will need work

    QString name() const { return m_name; }
    Factory* parent() const { return m_parent; }
    FactoryNode* node() const { return m_node; } // nullptr for root

    const QList<AbstractNode*>& subNodes() const { return m_subNodes; }
    const QList<Factory*>& subFactories() const { return m_subFactories; }

    void addNode(AbstractNode& node);
    void removeNode(AbstractNode& node);

    // const QList<Port*>& inputs() const { return m_inputs; }
    // const QList<Port*>& outputs() const { return m_outputs; }
};
