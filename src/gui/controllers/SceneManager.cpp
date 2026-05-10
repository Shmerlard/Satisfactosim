#include "SceneManager.h"
#include "SceneModel.h"
#include "RecipeListModel.h"

SceneManager::SceneManager(SessionManager* session, QObject* parent)
    : QObject(parent)
{
    m_session = (!session) ? &SessionManager::get() : session;
    m_model = new SceneModel(this, this);
    m_recipeModel = new RecipeListModel(this);

    connect(m_session, &SessionManager::nodeAdded,      this, &SceneManager::onNodeAdded);
    connect(m_session, &SessionManager::nodeRemoved,    this, &SceneManager::onNodeRemoved);
    connect(m_session, &SessionManager::factoryChanged, this, [this](Factory* f){
        m_model->loadFromFactory(f);
        emit factoryChanged();
    });

    m_model->loadFromFactory(m_session->activeFactory());
}

void SceneManager::onNodeAdded(AbstractNode* node)
{
    m_model->addNode(node);
}

void SceneManager::onNodeRemoved(AbstractNode* node)
{
    m_model->removeNode(node);
}

void SceneManager::enterFactory(AbstractNode* factoryNode)
{
    // qDebug() << factoryNode->posX();
    if (factoryNode->type() != NodeType::Factory)
        return;
    FactoryNode* factoryNode_p = static_cast<FactoryNode*>(factoryNode);
    Factory* f = &factoryNode_p->factory();
    m_session->enterFactory(f);
}
