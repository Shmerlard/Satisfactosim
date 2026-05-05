#pragma once

#include "AbstractNode.h"
#include "Factory.h"

class FactoryNode : public AbstractNode {
    friend class SessionManager;
    friend class Factory;

private:
    explicit FactoryNode(Factory& parentFactory, Factory& ownedFactory, QString name = QString());
    Factory& m_factory;
public:
    ~FactoryNode() override = default;

    Factory& factory() const { return m_factory; }

    // int hierarchyLevel() const override { return 1; }
    float portRate(const Port* port) const override;
    QJsonObject getJsonNode() const override;

};
