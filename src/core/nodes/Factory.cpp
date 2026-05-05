#include "Factory.h"
#include "ExtractionNode.h"
#include "FactoryEdgeNode.h"
#include "FactoryNode.h"
#include "ProductionNode.h"

Factory::Factory(Factory* parent, QString name, QUuid id)
    : m_id(id.isNull() ? QUuid::createUuid() : id)
    , m_parent(parent)
    , m_name(name)
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

// void Factory::addNode(AbstractNode& node)
// {
//
//     if (m_subNodes.contains(&node))
//         return;
//
//     int newLevel = node.hierarchyLevel();
//     NodeType type = node.type();
//
//     switch (type) {
//     case NodeType::Abstract:
//         // ERROR
//         break;
//     case NodeType::FactoryEdge: {
//         auto* p = static_cast<FactoryEdgeNode*>(&node);
//         m_edgeNodes.push_back(p);
//         break;
//     }
//     case NodeType::Factory: {
//         auto* p = static_cast<FactoryNode*>(&node);
//         m_subFactories.append(&p->factory());
//         break;
//     }
//     case NodeType::Extraction:
//         break;
//     case NodeType::Production:
//         break;
//     default:
//         break;
//     }
//     // if (auto* p = dynamic_cast<FactoryNode*>(&node)) {
//     //     m_subFactories.append(&p->factory());
//     //     // FIX: maybe the order of the factories should match
//     //     // the order of factory nodes
//     // }
//     //
//     for (int i = 0; i < m_subNodes.size(); ++i) {
//         if (m_subNodes[i]->hierarchyLevel() > newLevel) {
//             m_subNodes.insert(i, &node);
//             return;
//         }
//     }
//     m_subNodes.append(&node);
// }

void Factory::removeNode(AbstractNode& node)
{
    // // if (m_subFactories.contains(node))
    // //     m_subFactories.removeAll(&node);
    // if (auto* p = dynamic_cast<FactoryNode*>(&node)) {
    //     m_subFactories.removeAll(&p->factory());
    // }
    // m_subNodes.removeAll(&node);
    for (auto it = m_subNodes.begin(); it != m_subNodes.end(); ++it) {
        if (it->get() == &node) {
            m_subNodes.erase(it);
            return;
        }
    }
}

// Factory::~Factory()
// {
//     qDeleteAll(m_subNodes);
//     qDeleteAll(m_subFactories);
// }
