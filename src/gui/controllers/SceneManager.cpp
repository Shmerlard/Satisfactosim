#include "SceneManager.h"
#include "RecipeListModel.h"
#include "SceneModel.h"
#include "core/nodes/Connection.h"
#include "core/nodes/Factory.h"
#include "core/nodes/MachineNode.h"
#include "core/nodes/Port.h"
#include "core/nodes/SplitterNode.h"

SceneManager::SceneManager(QObject* parent)
    : QObject(parent)
{
    m_session = &SessionManager::get();
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
    // connect(m_session, &SessionManager::machineLimitChanged, this, &SceneManager::onMachineLimitChanged);

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

void SceneManager::enterRootFactory()
{
    m_session->enterRootFactory();
}

AbstractNode* SceneManager::createMachineNode(const QString recipe, double x, double y, QPoint srcPort)
{
    AbstractNode* node = nullptr;
    const Recipe* recipe_p = GameLibrary::get().getRecipeByClass(recipe);
    if (auto* p = dynamic_cast<const ProductionRecipe*>(recipe_p)) {
        node = m_session->createProductionNode(*p);
    } else if (auto* p = dynamic_cast<const ExtractionRecipe*>(recipe_p)) {
        node = m_session->createExtractionNode(*p);
    } else {
        return nullptr;
    }
    node->setPosX(x);
    node->setPosY(y);

    if (srcPort.x() >= 0) {
        Port* src = m_session->activeFactory()->nodes().at(srcPort.x())->getPortFromIndex(srcPort.y());

        if (src && src->item) {
            auto& targets = src->type == PortType::Input ? node->outputs() : node->inputs();
            for (auto& p : targets) {
                if (p->item == src->item) {
                    m_session->connectNode(*src, *p);
                    break;
                }
            }
        }
    }
    return node;
}

void SceneManager::createSubFactory(const QString name, double x, double y)
{
    Factory* child = m_session->createFactory(m_session->activeFactory(), name);

    // FactoryNode* node = m_session->createFactoryNode(*m_session->activeFactory(), *child, name);
    FactoryNode* node = child->node();
    node->setPosX(x);
    node->setPosY(y);
}

void SceneManager::createSplitterNode(double x, double y, QPoint srcPort)
{
    SplitterNode* node = m_session->createSplitterNode({ Frac(1), Frac(1) }, m_session->activeFactory());
    if (node) {
        node->setPosX(x);
        node->setPosY(y);

        // FIX: maybe have the srcPort as an input to session mananger
        // to prevent emitting too many?
        if (srcPort.x() >= 0) {
            Port* src = m_session->activeFactory()->nodes().at(srcPort.x())->getPortFromIndex(srcPort.y());
            Port* target = nullptr;
            if (src->type == PortType::Input)
                target = node->outputs()[0].get();
            else
                target = node->inputs()[0].get(); // FIX: might be dangerours

            if (!target)
                return;

            m_session->connectNode(*src, *target);
        }

    }
}

void SceneManager::createEdgeNode(bool isInput, const QString name, double x, double y, QPoint srcPort)
{
    PortType edgeType = isInput ? PortType::Input : PortType::Output;
    FactoryEdgeNode* node = m_session->createFactoryEdgeNode(edgeType, m_session->activeFactory(), name);
    if (node) {
        node->setPosX(x);
        node->setPosY(y);
        if (srcPort.x() >= 0) {
            Port* src = m_session->activeFactory()->nodes().at(srcPort.x())->getPortFromIndex(srcPort.y());

            if (src && src->item) {
                m_session->connectNode(*src, *node->port());
            }
        }
    }
}

void SceneManager::deleteNode(AbstractNode* node)
{
    if (node)
        m_session->deleteNode(node);
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

void SceneManager::setMachineLimit(AbstractNode* node, float limit)
{
    if (node->isMachineNode())
        m_session->setMachineLimit(node, limit);
}

void SceneManager::incMachineLimit(AbstractNode* node, int inc)
{
    m_session->incMachineLimit(node, inc);
}

void SceneManager::setSomersloopCount(AbstractNode* node, int count)
{
    m_session->setMachineSomersloop(node, count);
}

void SceneManager::setOverclock(AbstractNode* node, float overclock)
{
    m_session->setMachineOverclock(node, overclock);
}

void SceneManager::setTier(ExtractionNode* node, int tier)
{
    m_session->setExtractionTier(node, tier);
}
void SceneManager::setPurity(ExtractionNode* node, NodePurity purity)
{
    m_session->setExtractionPurity(node, purity);
}

void SceneManager::solve()
{
    m_session->cleanSolve();
}

void SceneManager::save(const QString& path)
{
    m_session->save(QUrl(path).toLocalFile());
}

void SceneManager::load(const QString& path)
{
    m_session->load(QUrl(path).toLocalFile());
}

void SceneManager::deleteConnection(QObject* connObj)
{
    Connection* conn = qobject_cast<Connection*>(connObj);
    if (!conn)
        return;
    m_session->disconnectNode(*conn->srcPort(), *conn->dstPort());
}
