#include "SceneManager.h"
#include "gui/items/ProductionItem.h"

SceneManager::SceneManager(SessionManager* session, QObject* parent)
    : QObject(parent)
{
    m_session = (!session) ? &SessionManager::get() : session;
    connect(m_session, &SessionManager::nodeAdded, this, &SceneManager::onNodeAdded);
    connect(m_session, &SessionManager::nodeRemoved, this, &SceneManager::onNodeRemoved);
}

AbstractItem* SceneManager::itemFromNode(AbstractNode* node) {
    return m_nodeItemMap.value(node, nullptr);
}

void SceneManager::onNodeAdded(AbstractNode* node)
{
    if (!node)
        return;

    if (m_nodeItemMap.contains(node))
        return;
    m_nodeItemMap.insert(node, new AbstractItem(node, QPointF(0, 0), this));
}

void SceneManager::onNodeRemoved(AbstractNode* node)
{
    // DO STUFF
}

ProductionItem* SceneManager::createProductionItem(const Recipe& recipe, Factory* factory, QString name, QPointF pos)
{
    ProductionNode* node = SessionManager::get().createProductionNode(recipe, factory, name);
    if (!node)
        return nullptr;

    ProductionItem* item = new ProductionItem(node, pos, this);
    return item;
}
