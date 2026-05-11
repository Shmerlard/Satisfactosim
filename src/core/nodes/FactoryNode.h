#pragma once

#include "AbstractNode.h"
#include "Factory.h"

class FactoryNode : public AbstractNode {
    Q_OBJECT;

    friend class SessionManager;
    friend class Factory;

private:
    explicit FactoryNode(
            Factory& parentFactory,
            Factory& ownedFactory,
            QString name = QString());
    Factory& m_factory;
    QMap<Port*, FactoryEdgeNode*> m_portEdges;

public:
    Factory& factory() const { return m_factory; }

    void onPortConnected(Port& port) override;
    void onPortDisconnected(Port& port) override;
    Port* addEdgePort(FactoryEdgeNode* edge);
    float portRate(const Port* port) const override;

    QJsonObject getJsonNode() const override;
};
