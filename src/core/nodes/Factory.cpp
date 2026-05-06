#include "Factory.h"
#include "ExtractionNode.h"
#include "FactoryEdgeNode.h"
#include "FactoryNode.h"
#include "ProductionNode.h"

Factory::Factory(Factory* parent, QString name, QUuid id)
    : m_id(id.isNull() ? QUuid::createUuid() : id)
    , m_parent(parent)
    , m_name(name)
    , m_node(nullptr)
// , m_subNodes(QList<AbstractNode*>())
{
}

ProductionNode* Factory::createProductionNode(const Recipe& recipe, QString name)
{
    // std::unique_ptr<ProductionNode> node(new ProductionNode(*this, recipe, name));
    // ProductionNode* ptr = node.get();
    // m_subNodes.push_back(std::unique_ptr<AbstractNode>(std::move(node)));
    // return ptr;

    ProductionNode* ptr = new ProductionNode(*this, recipe, name);
    m_subNodes.push_back(std::unique_ptr<AbstractNode>(ptr));
    return ptr;
}

ExtractionNode* Factory::createExtractionNode(const ExtractionRecipe& recipe, int tier, QString name)
{
    ExtractionNode* ptr = new ExtractionNode(*this, recipe, 0, name);
    m_subNodes.push_back(std::unique_ptr<AbstractNode>(ptr));
    return ptr;
}

Factory* Factory::createFactory(QString name)
{
    Factory* facPtr = new Factory(this, name);
    FactoryNode* facNodePtr = new FactoryNode(*this, *facPtr, name);
    m_subNodes.push_back(std::unique_ptr<AbstractNode>(facNodePtr));
    m_subFactories.push_back(std::unique_ptr<Factory>(facPtr));
    return facPtr;
}

FactoryEdgeNode* Factory::createFactoryEdgeNode(PortType edgeType, QString name)
{
    if (!m_parent) {
        return nullptr;
        // ADD ERROR MESSAGE
    }
    FactoryEdgeNode* edge = new FactoryEdgeNode(*this, edgeType, name);
    m_subNodes.push_back(std::unique_ptr<AbstractNode>(edge));
    return edge;
}

void Factory::removeNode(AbstractNode& node)
{
    for (auto it = m_subNodes.begin(); it != m_subNodes.end(); ++it) {
        if (it->get() == &node) {
            m_subNodes.erase(it);
            return;
        }
    }
}
