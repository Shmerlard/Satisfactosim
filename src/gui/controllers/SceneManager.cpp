#include "SceneManager.h"
#include "SceneModel.h"

SceneManager::SceneManager(SessionManager* session, QObject* parent)
    : QObject(parent)
{
    m_session = (!session) ? &SessionManager::get() : session;
    m_model = new SceneModel(this, this);

    connect(m_session, &SessionManager::nodeAdded,   this, &SceneManager::onNodeAdded);
    connect(m_session, &SessionManager::nodeRemoved, this, &SceneManager::onNodeRemoved);
    connect(m_session, &SessionManager::activeFactoryChanged, this, [this](){
        m_model->loadFromFactory(m_session->activeFactory());
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

