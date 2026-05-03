#include "SessionManager.h"
#include "GameLibrary.h"
#include "core/nodes/ProductionNode.h"

ProductionNode* SessionManager::createProductionNode(const Recipe& recipe, Factory* factory, QString name)
{
    // TODO: signature takes const Recipe& but ProductionNode constructor expects const ProductionRecipe& — type mismatch, won't compile
    Factory* f = factory ? factory : m_activeFactory;
    ProductionNode* node = new ProductionNode(*f, recipe, name);
    emit nodeAdded(node);
    return node;
}

// ProductionNode* SessionManager::createProductionNode(const QString& recipe, Factory* factory, QString name)
// {
//
// }

ProductionNode* SessionManager::createProductionNodeByClass(const QString& rClass, Factory* factory, QString name)
{
    // TODO: getRecipeByClass returns const Recipe*, but createProductionNode needs const ProductionRecipe& — needs a cast or a typed getProductionRecipeByClass
    const Recipe* r = GameLibrary::get().getRecipeByClass(rClass);
    if (!r)
        return nullptr;

    return createProductionNode(*r, factory, name);
}

ExtractionNode* SessionManager::createExtractionNode(const ExtractionRecipe* recipe, int tier, Factory* factory, QString name)
{
    Factory* f = factory ? factory : m_activeFactory;
    if (!recipe)
        return nullptr;
    ExtractionNode* node = new ExtractionNode(*f, *recipe, tier, name);
    emit nodeAdded(node);
    return node;
}

ExtractionNode* SessionManager::createExtractionNodeByName(QString resourceName, int tier, Factory* factory, QString name)
{
    const ExtractionRecipe* r = GameLibrary::get().getExtRecipeByResource(resourceName);
    if (!r)
        return nullptr;
    return createExtractionNode(r, tier, factory, name);
}

Factory* SessionManager::createFactory(Factory* parent, QString name)
{
    Factory* p = parent ? parent : m_activeFactory;
    Factory* factory = new Factory(p, name);
    FactoryNode* node = new FactoryNode(*p, *factory, name);
    emit nodeAdded(node);
    return factory;
}
void SessionManager::enterFactory(Factory* f)
{
    if (!f)
        return;
    m_activeFactory = f;
    emit factoryChanged(f);
    // emit activeFactoryChanged();
}

// FIX: maybe move this function to be internal
void SessionManager::connectNode(Port* src, Port* dest)
{
    if (!src || !dest)
        return;
    int srcIdx = src->owner.getPortIndex(*src);
    int destIdx = dest->owner.getPortIndex(*dest);
    QString err;
    if (src->owner.connectToPort(*src, *dest, &err)) {
        qDebug() << "Connected:" << src->owner.name() << "[port" << srcIdx << "]"
                 << "->" << dest->owner.name() << "[port" << destIdx << "]";
        emit portsConnected(&src->owner, srcIdx, &dest->owner, destIdx);
        emit nodeConnected();
        return;
    }
    qWarning() << "Connect failed:" << err;
}

void SessionManager::connectNode(int srcNode, int srcPort, int dstNode, int dstPort)
{
    AbstractNode* srcNode_p = m_activeFactory->subNodes().value(srcNode);
    AbstractNode* dstNode_p = m_activeFactory->subNodes().value(dstNode);
    if (!srcNode_p || !dstNode_p) {
        qWarning() << "Invalid Source or Destination Node";
        return;
    }
    Port* srcPort_p = srcNode_p->getPortFromIndex(srcPort);
    Port* dstPort_p = dstNode_p->getPortFromIndex(dstPort);
    if (!srcPort_p || !dstPort_p) {
        qWarning() << "Invalid Source or Destination Port";
        return;
    }
    connectNode(srcPort_p, dstPort_p);
}

void SessionManager::disconnectNode(Port* src, Port* dest)
{
    if (!src) {
        qWarning() << "No Source Port Found";
        return;
    }

    Port* _dest; // for working with implicit dest port
    if (!dest) {
        if (src->connectedTo.size() != 1) {
            qWarning() << "More then one!";
            return;
        }
        _dest = src->connectedTo.value(0);
    } else {
        _dest = dest;
    }

    if (!src->connectedTo.contains(_dest) || !_dest->connectedTo.contains(src)) {
        qWarning() << "Ports are not connected!";
        return;
    }

    // TODO: && short-circuits — if first removeOne succeeds but second fails, src is modified but _dest is not, leaving a half-disconnected state
    if (!src->connectedTo.removeOne(_dest) || !_dest->connectedTo.removeOne(src)) {
        qWarning() << "Error removing!";
        return;
    }
    qDebug() << "Disconnected!";
}

void SessionManager::disconnectNode(int srcNode, int srcPort, int dstNode, int dstPort)
{
    AbstractNode* srcNode_p = m_activeFactory->subNodes().value(srcNode);
    if (!srcNode_p) {
        qWarning() << "Invalid Source Node";
        return;
    }
    Port* srcPort_p = srcNode_p->getPortFromIndex(srcPort);

    if (dstNode == -1 || dstPort == -1) {
        // if (srcPort_p && srcPort_p->connectedTo.size() == 1) {
        disconnectNode(srcPort_p, nullptr);
        return;
        // }
    }
    AbstractNode* dstNode_p = m_activeFactory->subNodes().value(dstNode);
    if (!dstNode_p) {
        qWarning() << "Invalid Source or Destination Node";
        return;
    }
    Port* dstPort_p = dstNode_p->getPortFromIndex(dstPort);
    if (!srcPort_p || !dstPort_p) {
        qWarning() << "Invalid Source or Destination Port";
        return;
    }
    disconnectNode(srcPort_p, dstPort_p);
}

void SessionManager::renameNode(int index, QString name)
{
    if (name.isEmpty()) {
        qWarning() << "Invalid Name: " << name;
        return;
    }

    // FIX: boundry check
    AbstractNode* node = m_activeFactory->subNodes().value(index);
    if (!node) {
        qWarning() << "Invalid Index: " << index;
        return;
    }
    node->setName(name);
    qDebug() << "Node renamed to " << name;
}

void SessionManager::deleteNode(AbstractNode* node)
{
    // FIXME: fix implementation!
    if (!node)
        return;
    if (node->parentFactory())
        node->parentFactory()->removeNode(*node);
    emit nodeRemoved(node);
    delete node;
}

void SessionManager::setExtractionTier(AbstractNode* node, int tier)
{
    if (auto* en = dynamic_cast<ExtractionNode*>(node))
        en->m_tier = tier;
}

void SessionManager::setExtractionPurity(AbstractNode* node, NodePurity purity)
{
    if (auto* en = dynamic_cast<ExtractionNode*>(node))
        en->setPurity(purity);
}

void SessionManager::setMachineLimit(AbstractNode* node, float limit)
{
    if (auto* mn = dynamic_cast<MachineNode*>(node))
        mn->setMachineLimit(limit);
}
