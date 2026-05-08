#include "SessionManager.h"
#include "GameLibrary.h"
#include "core/nodes/ProductionNode.h"
#include "src/core/nodes/Factory.h"
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <queue>

namespace {
QJsonObject serializeFactory(const Factory& factory)
{
    QJsonObject obj;
    obj["id"] = factory.id().toString();
    obj["name"] = factory.name();

    QJsonArray nodesArray;
    for (const auto& node : factory.subNodes())
        nodesArray.append(node->getJsonNode());
    obj["nodes"] = nodesArray;

    QJsonArray connections;
    for (const auto& node : factory.subNodes()) {
        for (const auto& port : node->outputs()) {
            for (Port* peer : port->connectedTo) {
                QJsonObject conn;
                conn["from"] = QJsonArray { node->id().toString(), node->getPortIndex(*port) };
                conn["to"] = QJsonArray { peer->owner.id().toString(), peer->owner.getPortIndex(*peer) };
                connections.append(conn);
            }
        }
    }
    obj["connections"] = connections;
    QJsonArray subFactories;
    for (const auto& subFactory : factory.subFactories())
        subFactories.append(serializeFactory(*subFactory));
    obj["sub_factories"] = subFactories;

    return obj;
}

// Factory* deserializeFactory(const QJsonObject obj, Factory* parent, QMap<QString, AbstractNode*>& nodeMap)
// {
//     QUuid factoryId = QUuid(obj["id"].toString());
//     QString name = obj["name"].toString();
//
//     Factory* factory = new Factory(parent, name, factoryId);
//
// QMap<QString, Factory*> subFactoryById;
// for (const QJsonValue& v : obj["sub_factories"].toArray()) {
//     Factory* sub = deserializeFactory(v.toObject(), factory, nodeMap);
//     subFactoryById[sub->id().toString()] = sub;
// }
//
// for (const QJsonValue& v : obj["nodes"].toArray()) {
//     QJsonObject n = v.toObject();
//     QString id = n["id"].toString();
//     NodeType type = static_cast<NodeType>(n["type"].toInt());
//     QString nname = n["name"].toString();
//     QUuid nodeId = QUuid(id);
//     AbstractNode* node = nullptr;
//
//     if (type == NodeType::Production) {
//         float limit = (float)n["machineLimit"].toDouble(-1.0);
//         const QString recipe = n["recipe"].toString();
//         auto* pn = SessionManager::get().createProductionNodeByClass(recipe, factory, nname);
//         pn->setId(nodeId);
//         pn->setMachineLimit(limit);
//         node = pn;
//     } else if (type == NodeType::Extraction) {
//         float limit = (float)n["machineLimit"].toDouble(-1.0);
//         const QString recipe = n["recipe"].toString();
//         int tier = n["tier"].toInt();
//         NodePurity purity = static_cast<NodePurity>(n["purity"].toInt());
//         auto* en = SessionManager::get().createExtractionNodeByClass(recipe, tier, factory, nname);
//         en->setId(nodeId);
//         en->setMachineLimit(limit);
//         node = en;
//     } else if (type == NodeType::Production) {
//         QString factoryId = n["factoryId"].toString();
//         Factory* sub = subFactoryById.value(factoryId);
//         if (!sub) {
//             // ERROR
//             continue;
//         }
//         auto* fn = SessionManager::get().createFactoryNode(*factory, *sub, nname);
//         node = fn;
//
//     } else {
//     }
//
//     if (node)
//         nodeMap[id] = node;
// }
//
//     return factory;
// }

// void deserializeFactory(
//     std::pair<Factory*, QJsonObject>& currentFactory,
//     std::queue<std::pair<Factory*, QJsonObject>>& pendingFactories)
// {
//     Factory* factory = currentFactory.first;
//     QJsonObject* json = &currentFactory.second;
//
//     // QUuid factoryId = QUuid(currentFactory.second["id"].toString());
//     QUuid factoryId = QUuid(json->value("id").toString());
//     factory->setId(factoryId);
//     factory->setName(json->value("name").toString());
//
//     // FIX: maybe too much duplication of the text objects happening?
//     // pass by value of json?
//     for (auto subFactoryJson : json->value("sub_factories").toArray()) {
//         Factory* newSub = factory->createFactory();
//     }
//
//     // 1. create a factory
//     // set it's id
//     // add to queue it's sub factories
//     // creates it's nodes
//     // connections
// }
} // namespace

void SessionManager::save(const QString& path)
{
    // TODO: add compression?
    // remove default and empty values
    QJsonObject root;
    root["metadata"] = QJsonObject { { "version", 1 } };
    root["root_factory"] = serializeFactory(*m_rootFactory);

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        emit operationFailed("Could not open file " + path);
        return;
    }
    file.write(QJsonDocument(root).toJson());
}

void SessionManager::load(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        emit operationFailed("Could not open file " + path);
        return;
    }
    // CLEAN CURRENT FACTORIES

    QJsonParseError parserError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parserError);

    if (parserError.error != QJsonParseError::NoError) {
        emit operationFailed("Json Parser Error: " + parserError.errorString());
        return;
    }

    QJsonObject root = doc.object();
    std::queue<std::pair<Factory*, QJsonObject>> pendingFactories;
    QMap<QUuid, AbstractNode*> uuidMap;
    QJsonObject rootFactoryJson = root["root_factory"].toObject();
    // m_rootFactory = new Factory(nullptr, QString("Main Factory"));
    auto newRoot = std::make_unique<Factory>(nullptr, QString("Main Factory"));
    pendingFactories.push({ newRoot.get(), rootFactoryJson });

    while (!pendingFactories.empty()) {
        auto pair = pendingFactories.front();
        pendingFactories.pop();
        if (!deserializeFactory(pair, pendingFactories)) {
            emit operationFailed("Failed to load Factory");
            return;
        }
    }

    m_rootFactory = std::move(newRoot);
    m_activeFactory = m_rootFactory.get();
}

FactoryEdgeNode* SessionManager::createFactoryEdgeNode(PortType edgeType, Factory* parentFactory, QString name)
{
    Factory* f = parentFactory ? parentFactory : m_activeFactory;
    FactoryEdgeNode* node = f->createFactoryEdgeNode(edgeType);
    emit nodeAdded(node);
    return node;
}

ProductionNode* SessionManager::createProductionNode(const Recipe& recipe, Factory* factory, QString name)
{
    Factory* f = factory ? factory : m_activeFactory;
    ProductionNode* node = f->createProductionNode(recipe, name);
    emit nodeAdded(node);
    return node;
}

ProductionNode* SessionManager::createProductionNodeByClass(const QString& rClass, Factory* factory, QString name)
{
    const Recipe* r = GameLibrary::get().getRecipeByClass(rClass);
    if (!r)
        return nullptr;

    Factory* f = factory ? factory : m_activeFactory;
    return f->createProductionNode(*r, name);
}

ExtractionNode* SessionManager::createExtractionNode(const ExtractionRecipe* recipe, int tier, Factory* factory, QString name)
{
    Factory* f = factory ? factory : m_activeFactory;
    if (!recipe)
        return nullptr;
    ExtractionNode* node = f->createExtractionNode(*recipe, tier, name);
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

ExtractionNode* SessionManager::createExtractionNodeByClass(QString rClass, int tier, Factory* factory, QString name)
{

    const Recipe* r = GameLibrary::get().getRecipeByClass(rClass);
    const ExtractionRecipe* er = dynamic_cast<const ExtractionRecipe*>(r);
    if (!er)
        return nullptr;
    return createExtractionNode(er, tier, factory, name);
}

Factory* SessionManager::createFactory(Factory* parent, QString name)
{
    Factory* p = parent ? parent : m_activeFactory;
    // Factory* factory = new Factory(p, name);
    // FactoryNode* node = new FactoryNode(*p, *factory, name);
    Factory* newFactory = p->createFactory(name);
    emit nodeAdded(newFactory->node());
    return newFactory;
}

FactoryNode* SessionManager::createFactoryNode(Factory& parent, Factory& owned, QString name)
{
    FactoryNode* node = new FactoryNode(parent, owned, name);
    emit nodeAdded(node);
    return node;
}

void SessionManager::enterFactory(Factory* f)
{
    if (!f)
        return;
    m_activeFactory = f;
    emit factoryChanged(f);
    // emit activeFactoryChanged();
}

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
    AbstractNode* srcNode_p = m_activeFactory->subNodes().at(srcNode).get();
    AbstractNode* dstNode_p = m_activeFactory->subNodes().at(dstNode).get();
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
        emit operationFailed("No Source Port Found");
        return;
    }
    QString err;
    src->owner.disconnectPort(src, dest, &err);
    if (!err.isEmpty()) {
        emit operationFailed(err);
        return;
    }
    emit nodeDisconnected();
}

void SessionManager::disconnectNode(int srcNode, int srcPort, int dstNode, int dstPort)
{
    AbstractNode* srcNode_p = m_activeFactory->subNodes().at(srcNode).get();
    if (!srcNode_p) {
        qWarning() << "Invalid Source Node";
        return;
    }
    Port* srcPort_p = srcNode_p->getPortFromIndex(srcPort);

    if (dstNode == -1 || dstPort == -1) {
        disconnectNode(srcPort_p, nullptr);
        return;
    }
    AbstractNode* dstNode_p = m_activeFactory->subNodes().at(dstNode).get();
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
    AbstractNode* node = m_activeFactory->subNodes().at(index).get();
    if (!node) {
        qWarning() << "Invalid Index: " << index;
        return;
    }
    node->setName(name);
    qDebug() << "Node renamed to " << name;
}

void SessionManager::deleteNode(AbstractNode* node)
{
    if (!node)
        return;
    emit nodeRemoved(node);
    if (node->parentFactory())
        node->parentFactory()->removeNode(*node);
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

bool SessionManager::deserializeFactory(
    std::pair<Factory*, QJsonObject>& currentFactory,
    std::queue<std::pair<Factory*, QJsonObject>>& pendingFactories)
{
    Factory* factory = currentFactory.first;
    QJsonObject* json = &currentFactory.second;

    QUuid factoryId = QUuid(json->value("id").toString());
    factory->setId(factoryId);
    factory->setName(json->value("name").toString());

    // FIX: maybe too much duplication of the text objects happening?
    // pass by value of json?
    for (auto subFactoryJson : json->value("sub_factories").toArray()) {
        Factory* newSub = factory->createFactory();
        pendingFactories.push({ newSub, subFactoryJson.toObject() });
    }

    for (auto node : json->value("nodes").toArray()) {
        QJsonObject nodeObject = node.toObject();
        QUuid nodeId = QUuid(nodeObject.value("id").toString());
        QString nodeName = nodeObject["name"].toString();
        NodeType nodeType = static_cast<NodeType>(nodeObject["type"].toInt());

        switch (nodeType) {
        case NodeType::Extraction: {
            QString recipeString = nodeObject["recipe"].toString();
            const ExtractionRecipe* nodeRecipe = GameLibrary::get().getExtRecipeByClass(recipeString);
            if (!nodeRecipe) {
                qWarning() << "COULDNT FIND NODE RECIPE: " << recipeString;
                return false;
            }
            int nodeTier = nodeObject["tier"].toInt();
            ExtractionNode* newNode = factory->createExtractionNode(*nodeRecipe,nodeTier, nodeName);
            NodePurity nodePurity = static_cast<NodePurity>(nodeObject["purity"].toInt());
            newNode->setPurity(nodePurity);
            newNode->setId(nodeId);
            break;
        }
        case NodeType::Production: {
            QString recipeString = nodeObject["recipe"].toString();
            // FIX: THIS COULD BE DANGEROUS
            // const ProductionRecipe* nodeRecipe = static_cast<const ProductionRecipe*>(GameLibrary::get().getRecipeByClass(recipeString));
            const Recipe* nodeRecipe = GameLibrary::get().getRecipeByClass(recipeString);
            
            if (!nodeRecipe) {
                qWarning() << "COULDNT FIND NODE RECIPE: " << recipeString;
                return false;
            }
            ProductionNode* newNode = factory->createProductionNode(*nodeRecipe, nodeName);
            newNode->setId(nodeId);
            break;
        }
        case NodeType::FactoryEdge: {
            
            break;
        }
        default: {
            break;
        }
        }
    }
        return true;

    // 1. create a factory
    // set it's id
    // add to queue it's sub factories
    // creates it's nodes
    // connections
}
