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
    void disconnectPort(Port* port, Port* peer, QString* err = nullptr) override;
    PortType edgeType() const { return m_edgeType; }
    float portRate(const Port* port) const override;
    QJsonObject getJsonNode() const override;
};
