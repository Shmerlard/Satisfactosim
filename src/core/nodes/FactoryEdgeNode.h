#pragma once

#include "AbstractNode.h"

class FactoryEdgeNode : public AbstractNode {
    friend class SessionManager;

private:
    PortType m_edgeType;
    explicit FactoryEdgeNode(
        Factory& parentFactory,
        PortType edgeType,
        QString name = QString(),
        QUuid id = QUuid());
};
