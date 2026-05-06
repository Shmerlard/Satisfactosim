#pragma once

#include "Port.h"
#include "core/types/Types.h"
#include <QList>
#include <QString>
#include <QUuid>

class AbstractNode;
class FactoryNode;
class FactoryEdgeNode;
class ProductionNode;
class ExtractionNode;
struct Recipe;

class Factory {
private:
    friend class SessionManager;
    friend class FactoryNode;

    QUuid m_id;
    QString m_name;
    Factory* m_parent;
    FactoryNode* m_node;
    // QList<Factory*> m_subFactories;
    std::vector<std::unique_ptr<Factory>> m_subFactories;
    std::vector<std::unique_ptr<AbstractNode>> m_subNodes;
    // std::vector<std::unique_ptr<FactoryEdgeNode>> m_edgeNodes;

private:

    Factory*         createFactory(QString name = QString());
    FactoryEdgeNode* createFactoryEdgeNode(PortType edgeType, QString name = QString());
    ExtractionNode*  createExtractionNode(const ExtractionRecipe& recipe, int tier = 1, QString name = QString());
    ProductionNode*  createProductionNode(const Recipe& recipe, QString name = QString());
public:
    explicit Factory(Factory* parent, QString name, QUuid id = QUuid());
    QUuid id() const { return m_id; }
    // ~Factory(); // FIXME: will need work

    QString name() const { return m_name; }
    Factory* parent() const { return m_parent; }
    FactoryNode* node() const { return m_node; } // nullptr for root

    // const QList<AbstractNode*>& subNodes() const { return m_subNodes; }
    // const QList<Factory*>& subFactories() const { return m_subFactories; }
    const std::vector<std::unique_ptr<Factory>>& subFactories() const { return m_subFactories; }
    const std::vector<std::unique_ptr<AbstractNode>>& subNodes() const { return m_subNodes; }


    // void addNode(AbstractNode& node);
    void removeNode(AbstractNode& node);

};
