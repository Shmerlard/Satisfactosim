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
    for (const auto& node : factory.subNodes()) {
        if (node->type() == NodeType::FactoryEdge)
            continue;
        // FIX: move into more seperated approach
        nodesArray.append(node->getJsonNode());
    }
    obj["nodes"] = nodesArray;

    QJsonArray edgesArray;
    for (const auto& edge : factory.edges())
        edgesArray.append(edge->getJsonNode());
    obj["edges"] = edgesArray;

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
    // auto serializeConnections = [&](const AbstractNode* node) {
    //     for (const auto& port : node->outputs()) {
    //         for (Port* peer : port->connectedTo) {
    //             QJsonObject conn;
    //             conn["from"] = QJsonArray { node->id().toString(), node->getPortIndex(*port) };
    //             conn["to"] = QJsonArray { peer->owner.id().toString(),
    //                 peer->owner.getPortIndex(*peer) };
    //             connections.append(conn);
    //         }
    //     }
    // };
    // for (const auto& node : factory.subNodes())
    //     serializeConnections(node.get());
    // for (const auto& edge : factory.edges())
    //     serializeConnections(edge.get());
    obj["connections"] = connections;

    QJsonArray subFactories;
    for (const auto& subFactory : factory.subFactories())
        subFactories.append(serializeFactory(*subFactory));
    obj["sub_factories"] = subFactories;

    return obj;
}

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
    ProductionNode* node = f->createProductionNode(*r, name);
    emit nodeAdded(node);
    return node;
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
    QMap<QUuid, AbstractNode*> uuidMap;
    QUuid factoryId = QUuid(json->value("id").toString());
    factory->setId(factoryId);
    factory->setName(json->value("name").toString());

    // FIX: maybe too much duplication of the text objects happening?
    // pass by value of json?
    for (auto subFactoryJson : json->value("sub_factories").toArray()) {
        QJsonObject subFactObject = subFactoryJson.toObject();
        Factory* newSub = factory->createFactory();
        // FIX: add check
        newSub->setId(QUuid(subFactObject["id"].toString()));
        pendingFactories.push({ newSub, subFactObject });
    }

    // ------------ creating edges ---------------------
    auto edgesArray = json->value("edges").toArray();
    std::vector<QJsonObject> edgeVec;
    for (auto e : edgesArray)
        edgeVec.push_back(e.toObject());
    std::sort(edgeVec.begin(), edgeVec.end(), [](const QJsonObject& a, const QJsonObject& b) {
        return a["portIndex"].toInt() < b["portIndex"].toInt();
    });
    for (auto& edgeObject : edgeVec) {
        QUuid edgeId = QUuid(edgeObject["id"].toString());
        QString edgeName = edgeObject["name"].toString();
        PortType edgeType = static_cast<PortType>(edgeObject["edgeType"].toInt());
        FactoryEdgeNode* newEdge = factory->createFactoryEdgeNode(edgeType, edgeName);
        newEdge->setId(edgeId);
        uuidMap.insert(edgeId, newEdge);
    }

    // ------------ creating nodes ---------------------
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
            ExtractionNode* newNode = factory->createExtractionNode(*nodeRecipe, nodeTier, nodeName);
            NodePurity nodePurity = static_cast<NodePurity>(nodeObject["purity"].toInt());
            float limit = nodeObject["machineLimit"].toDouble();
            newNode->setPurity(nodePurity);
            newNode->setId(nodeId);
            newNode->setMachineLimit(limit);
            uuidMap.insert(nodeId, newNode);
            break;
        }
        case NodeType::Production: {
            QString recipeString = nodeObject["recipe"].toString();
            const Recipe* nodeRecipe = GameLibrary::get().getRecipeByClass(recipeString);

            if (!nodeRecipe) {
                qWarning() << "COULDNT FIND NODE RECIPE: " << recipeString;
                return false;
            }
            float limit = nodeObject["machineLimit"].toDouble();
            ProductionNode* newNode = factory->createProductionNode(*nodeRecipe, nodeName);
            newNode->setId(nodeId);
            newNode->setMachineLimit(limit);
            uuidMap.insert(nodeId, newNode);
            break;
        }
        // case NodeType::FactoryEdge: {
        //     PortType edgeType = static_cast<PortType>(nodeObject["edgeType"].toInt());
        //     FactoryEdgeNode* newNode = factory->createFactoryEdgeNode(edgeType, nodeName);
        //     newNode->setId(nodeId);
        //     break;
        // }
        case NodeType::Factory: {
            QUuid factoryId = QUuid(nodeObject["factoryId"].toString());
            FactoryNode* matchingNode = nullptr;
            for (auto& subNode : factory->subNodes()) {
                if (subNode->type() == NodeType::Factory) {
                    auto* fn = static_cast<FactoryNode*>(subNode.get());
                    if (fn->factory().id() == factoryId) {
                        matchingNode = fn;
                        break;
                    }
                }
            }
            if (!matchingNode) {
                qWarning() << "ERROR FINDING FACTORY" << factoryId.toString();
                break;
            }
            matchingNode->setId(nodeId);
            matchingNode->setName(nodeName);
            uuidMap.insert(nodeId, matchingNode);
            // FIX: maybe it would be better to add another function for creating a factory without subfactorynode
            // and just create a node later;
            break;
        }
        default: {
            qWarning() << "WARNING: UNABLE TO LOAD A NODE" << nodeId.toString();
            break;
        }
        }
    }

    // ------------ creating connections ---------------------
    auto connectionsArray = json->value("connections").toArray();
    for (auto connectionJson : connectionsArray) {
        auto connection = connectionJson.toObject();
        QJsonArray from = connection["from"].toArray();
        QJsonArray to = connection["to"].toArray();

        AbstractNode* fromNode = uuidMap.value(QUuid(from[0].toString()));
        AbstractNode* toNode = uuidMap.value(QUuid(to[0].toString()));
        if (!fromNode || !toNode) {
            qWarning() << "Connection references unknown node";
            continue;
        }

        Port* srcPort = fromNode->getPortFromIndex(from[1].toInt());
        Port* dstPort = toNode->getPortFromIndex(to[1].toInt());
        if (!srcPort || !dstPort) {
            qWarning() << "Connection references invalid port";
            continue;
        }

        fromNode->connectToPort(*srcPort, *dstPort);
    }
    // for (auto connectionJson : connectionsArray) {
    //     auto connection = connectionJson.toObject();
    //     QUuid fromId = QUuid(connection.value("from").toArray()[0].toString());
    //     int fromIndex = connection.value("from").toArray()[1].toInt();
    //     QUuid toId = QUuid(connection.value("to").toArray()[0].toString());
    //     int toIndex = connection.value("to").toArray()[1].toInt();
    //
    //
    // }

    return true;
}
