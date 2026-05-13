#include "SceneManager.h"
#include "RecipeListModel.h"
#include "SceneModel.h"
#include "core/nodes/Connection.h"
#include "core/nodes/Factory.h"

SceneManager::SceneManager(SessionManager* session, QObject* parent)
    : QObject(parent)
{
    m_session = (!session) ? &SessionManager::get() : session;
    m_model = new SceneModel(this, this);
    m_recipeModel = new RecipeListModel(this);
    m_recipeFilterModel = new RecipeFilterModel(this);
    m_recipeFilterModel->setSourceModel(m_recipeModel);

    connect(m_session, &SessionManager::nodeAdded, this, &SceneManager::onNodeAdded);
    connect(m_session, &SessionManager::nodeRemoved, this, &SceneManager::onNodeRemoved);
    connect(m_session, &SessionManager::factoryChanged, this, [this](Factory* f) {
        m_model->loadFromFactory(f);
        loadConnections(f);
        emit factoryChanged();
    });
    connect(m_session, &SessionManager::nodeConnected, this, [this](AbstractNode*, AbstractNode*) {
        loadConnections(m_session->activeFactory());
    });
    connect(m_session, &SessionManager::nodeDisconnected, this, [this](AbstractNode*, AbstractNode*) {
        loadConnections(m_session->activeFactory());
    });

    m_model->loadFromFactory(m_session->activeFactory());
    loadConnections(m_session->activeFactory());
}

void SceneManager::loadConnections(Factory* f)
{
    m_connections.clear();
    for (Connection* c : f->connections())
        m_connections.append(QVariant::fromValue<QObject*>(c));
    emit connectionsChanged();
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
    if (factoryNode->type() != NodeType::Factory)
        return;
    FactoryNode* factoryNode_p = static_cast<FactoryNode*>(factoryNode);
    Factory* f = &factoryNode_p->factory();
    m_session->enterFactory(f);
}

void SceneManager::enterParentFactory()
{
    m_session->enterParentFactory();
}

// void SceneManager::createMachineNode(Recipe* recipe, double x, double y)
// {
//     AbstractNode* node = nullptr;
//     if (auto* p = dynamic_cast<const ProductionRecipe*>(recipe)) {
//         node = m_session->createProductionNode(*p);
//     } else if ( auto* p = dynamic_cast<const ExtractionRecipe*>(recipe)) {
//         node = m_session->createExtractionNode(p);
//     } else {
//         return;
//     }
//     node->setPosX(x);
//     node->setPosY(y);
//
// }
void SceneManager::createMachineNode(const QString recipe, double x, double y)
{
    AbstractNode* node = nullptr;
    const Recipe* recipe_p = GameLibrary::get().getRecipeByClass(recipe);
    if (auto* p = dynamic_cast<const ProductionRecipe*>(recipe_p)) {
        node = m_session->createProductionNode(*p);
    } else if (auto* p = dynamic_cast<const ExtractionRecipe*>(recipe_p)) {
        node = m_session->createExtractionNode(*p);
    } else {
        return;
    }
    node->setPosX(x);
    node->setPosY(y);
}

void SceneManager::createSubFactory(const QString name, double x, double y)
{
    Factory* child = m_session->createFactory(m_session->activeFactory(), name);

    // FactoryNode* node = m_session->createFactoryNode(*m_session->activeFactory(), *child, name);
    FactoryNode* node = child->node();
    node->setPosX(x);
    node->setPosY(y);
}

void SceneManager::createEdgeNode(bool isInput, const QString name, double x, double y)
{
    PortType edgeType = isInput ? PortType::Input : PortType::Output;
    FactoryEdgeNode* node = m_session->createFactoryEdgeNode(edgeType, m_session->activeFactory(), name);
    if (node) {
        node->setPosX(x);
        node->setPosY(y);

    }
}

void SceneManager::setPortOffset(int nodeIndex, int portIndex, QPointF offset)
{
    AbstractNode* node = m_session->activeFactory()->nodes().at(nodeIndex);
    if (!node)
        return;
    Port* port = node->getPortFromIndex(portIndex);
    if (!port)
        return;
    port->offset = offset;
    for (Connection* conn : port->connections) {
        if (conn->srcPort() == port)
            conn->setSrcOffset(offset);
        else
            conn->setDstOffset(offset);
    }
}

void SceneManager::connectNodes(int srcNodeIdx, int srcPortIdx, int dstNodeIdx, int dstPortIdx)
{
    m_session->connectNode(srcNodeIdx, srcPortIdx, dstNodeIdx, dstPortIdx);
}

void SceneManager::deleteConnection(QObject* connObj)
{
    Connection* conn = qobject_cast<Connection*>(connObj);
    if (!conn) return;
    m_session->disconnectNode(*conn->srcPort(), *conn->dstPort());
}


// QVariantMap SceneManager::portAtPosition(qreal x, qreal y, qreal tolerance)
// {
//     for (AbstractNode* node : m_session->activeFactory()->nodes()) {
//         auto check = [&](const std::vector<std::unique_ptr<Port>>& ports) -> QVariantMap {
//             for (const auto& port : ports) {
//                 qreal px = node->posX() + port->offset.x();
//                 qreal py = node->posY() + port->offset.y();
//                 if (qAbs(px - x) <= tolerance && qAbs(py - y) <= tolerance) {
//                     QVariantMap m;
//                     m["nodeIndex"] = node->index();
//                     m["portIndex"] = node->getPortIndex(*port);
//                     return m;
//                 }
//             }
//             return {};
//         };
//         auto result = check(node->inputs());
//         if (!result.isEmpty())
//             return result;
//         result = check(node->outputs());
//         if (!result.isEmpty())
//             return result;
//     }
//     return {};
// }
