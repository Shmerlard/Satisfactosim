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
{
}

ProductionNode* Factory::createProductionNode(const ProductionRecipe& recipe, QString name)
{
    ProductionNode* ptr = new ProductionNode(*this, recipe, name);
    addNode(std::unique_ptr<AbstractNode>(ptr));
    return ptr;
}

ExtractionNode* Factory::createExtractionNode(const ExtractionRecipe& recipe, int tier, QString name)
{
    ExtractionNode* ptr = new ExtractionNode(*this, recipe, tier, name);
    addNode(std::unique_ptr<AbstractNode>(ptr));
    return ptr;
}

Factory* Factory::createFactory(QString name)
{
    Factory* facPtr = new Factory(this, name);
    FactoryNode* facNodePtr = new FactoryNode(*this, *facPtr, name);
    addNode(std::unique_ptr<AbstractNode>(facNodePtr));

    m_subFactories.push_back(std::unique_ptr<Factory>(facPtr));
    return facPtr;
}

FactoryEdgeNode* Factory::createFactoryEdgeNode(PortType edgeType, QString name)
{
    if (!m_parent) {
        return nullptr;
        // FIX:ADD ERROR MESSAGE
    }
    FactoryEdgeNode* edge = new FactoryEdgeNode(*this, edgeType, name);
    m_edges.push_back(edge);
    addNode(std::unique_ptr<AbstractNode>(edge));
    return edge;
}

void Factory::addNode(std::unique_ptr<AbstractNode> node)
{
    auto it = std::find_if(m_subNodes.begin(), m_subNodes.end(), [&](const auto& n) {
        return n->type() > node->type();
    });
    m_subNodes.insert(it, std::move(node));
}

void Factory::removeNode(AbstractNode& node)
{
    for (auto it = m_subNodes.begin(); it != m_subNodes.end(); ++it) {
        if (it->get() == &node) {
            m_subNodes.erase(it);
            return;
        }
    }
    m_edges.erase(std::remove(m_edges.begin(), m_edges.end(), &node), m_edges.end());
}
