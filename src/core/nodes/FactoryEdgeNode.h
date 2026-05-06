#pragma once

#include "AbstractNode.h"

class FactoryEdgeNode : public AbstractNode {
    friend class SessionManager;
    friend class Factory;

private:
    explicit FactoryEdgeNode(
        Factory& parentFactory,
        PortType edgeType,
        QString name = QString(),
        QUuid id = QUuid());

    PortType m_edgeType;
    void buildPort();

public:
    PortType edgeType() const { return m_edgeType; }
    float portRate(const Port* port) const override;
    QJsonObject getJsonNode() const override;
};
