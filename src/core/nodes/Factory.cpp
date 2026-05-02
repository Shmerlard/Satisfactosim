#include "Factory.h"
#include "FactoryNode.h"

Factory::Factory(Factory* parent, QString name)
    : m_parent(parent)
    , m_name(name)
// , m_subNodes(QList<AbstractNode*>())
{
}

void Factory::addNode(AbstractNode& node)
{

    if (m_subNodes.contains(&node))
        return;

    int newLevel = node.hierarchyLevel();
    if (auto* p = dynamic_cast<FactoryNode*>(&node)) {
        m_subFactories.append(&p->factory());
        // FIX: maybe the order of the factories should match
        // the order of factory nodes
    }

    for (int i = 0; i < m_subNodes.size(); ++i) {
        if (m_subNodes[i]->hierarchyLevel() > newLevel) {
            m_subNodes.insert(i, &node);
            return;
        }
    }
    m_subNodes.append(&node);
}

void Factory::removeNode(AbstractNode& node)
{
    // FIX: check why & fixes it
    m_subNodes.removeAll(&node);
}
