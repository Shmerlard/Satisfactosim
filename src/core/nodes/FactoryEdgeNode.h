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
    Port* m_mirrorPort = nullptr;

public:
    Port* port() const;
    void disconnectPort(Port* port, Port* peer, QString* err = nullptr) override;
    bool connectToPort(Port& src, Port& dst, QString* err = nullptr) override;
    PortType edgeType() const { return m_edgeType; }
    void onPortConnected(Port& port) override;
    void onPortDisconnected(Port& port) override;
    float portRate(const Port* port) const override;
    QJsonObject getJsonNode() const override;
    void setMirrorPort(Port& port) { m_mirrorPort = &port; }
};
