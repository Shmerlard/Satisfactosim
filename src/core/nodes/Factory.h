#pragma once

#include "Port.h"
#include <QList>
#include <QString>
#include <QUuid>

class AbstractNode;
class FactoryNode;
class FactoryEdgeNode;

class Factory {
private:
    // friend class SessionManager;
    friend class FactoryNode;

    QUuid m_id;
    QString m_name;
    Factory* m_parent;
    FactoryNode* m_node;
    QList<AbstractNode*> m_subNodes; // wouldnt it be unique pointers?
    QList<Factory*> m_subFactories;
    std::vector<std::unique_ptr<FactoryEdgeNode*>> m_edgeNodes;

public:
    explicit Factory(Factory* parent, QString name, QUuid id = QUuid());
    QUuid id() const { return m_id; }
    ~Factory(); // FIXME: will need work

    QString name() const { return m_name; }
    Factory* parent() const { return m_parent; }
    FactoryNode* node() const { return m_node; } // nullptr for root

    const QList<AbstractNode*>& subNodes() const { return m_subNodes; }
    const QList<Factory*>& subFactories() const { return m_subFactories; }

    void addNode(AbstractNode& node);
    void removeNode(AbstractNode& node);

};
