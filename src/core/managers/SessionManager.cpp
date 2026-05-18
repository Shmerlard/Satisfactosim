#include "SessionManager.h"
#include "GameLibrary.h"
#include "core/nodes/ExtractionNode.h"
#include "core/nodes/ProductionNode.h"
#include "src/core/nodes/Factory.h"
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <queue>

// ---------- NODE CREATION ---------------
FactoryEdgeNode* SessionManager::createFactoryEdgeNode(PortType edgeType, Factory* parentFactory, QString name)
{
    Factory* f = parentFactory ? parentFactory : m_activeFactory;
    QString err;
    FactoryEdgeNode* node = f->createFactoryEdgeNode(edgeType, name, &err);
    if (!node) {
        emit operationFailed(err);
        return nullptr;

    }
    emit nodeAdded(node);
    return node;
}

FactoryNode* SessionManager::createFactoryNode(Factory& parent, Factory& owned, QString name)
{
    FactoryNode* node = new FactoryNode(parent, owned, name);
    emit nodeAdded(node);
    return node;
}

ProductionNode* SessionManager::createProductionNode(const ProductionRecipe& recipe, Factory* factory, QString name)
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

    if (auto* p = dynamic_cast<const ProductionRecipe*>(r)) {
        Factory* f = factory ? factory : m_activeFactory;
        ProductionNode* node = f->createProductionNode(*p, name);
        emit nodeAdded(node);
        return node;
    }
    return nullptr;
}

ExtractionNode* SessionManager::createExtractionNode(const ExtractionRecipe& recipe, int tier, Factory* factory, QString name)
{
    Factory* f = factory ? factory : m_activeFactory;
    ExtractionNode* node = f->createExtractionNode(recipe, tier, name);
    emit nodeAdded(node);
    return node;
}

ExtractionNode* SessionManager::createExtractionNodeByName(QString resourceName, int tier, Factory* factory, QString name)
{
    const ExtractionRecipe* r = GameLibrary::get().getExtRecipeByResource(resourceName);
    if (!r)
        return nullptr;
    return createExtractionNode(*r, tier, factory, name);
}

// ExtractionNode* SessionManager::createExtractionNodeByClass(QString rClass, int tier, Factory* factory, QString name)
// {
//
//     const Recipe* r = GameLibrary::get().getRecipeByClass(rClass);
//     const ExtractionRecipe* er = dynamic_cast<const ExtractionRecipe*>(r);
//     if (!er)
//         return nullptr;
//     return createExtractionNode(er, tier, factory, name);
// }

void SessionManager::deleteNode(AbstractNode* node)
{
    if (!node)
        return;
    emit nodeRemoved(node);
    if (node->parentFactory())
        node->parentFactory()->removeNode(*node);
}

// ---------- FACTORY MANIPULATION ---------------
Factory* SessionManager::createFactory(Factory* parent, QString name)
{
    Factory* p = parent ? parent : m_activeFactory;
    Factory* newFactory = p->createFactory(name);
    emit nodeAdded(newFactory->node());
    return newFactory;
}

void SessionManager::enterFactory(Factory* f)
{
    if (!f)
        return;
    m_activeFactory = f;
    emit factoryChanged(f);
}

void SessionManager::enterRootFactory()
{
    enterFactory(m_rootFactory);
}

void SessionManager::enterParentFactory()
{
    if (m_activeFactory->parent())
        enterFactory(m_activeFactory->parent());
}

// ---------- CONNECTIONS ---------------
void SessionManager::connectNode(Port& src, Port& dst)
{
    QString err;
    Connection * conn = src.owner.parentFactory()->connect(src, dst, &err);
    if (conn)
        emit nodeConnected(&src.owner, &dst.owner);
    else
        emit operationFailed(err);
}

void SessionManager::connectNode(int srcNode, int srcPort, int dstNode, int dstPort, Factory* f)
{
    Factory* selectedFactory = f ? f : m_activeFactory;
    AbstractNode* srcNode_p = selectedFactory->nodes().value(srcNode);
    AbstractNode* dstNode_p = selectedFactory->nodes().value(dstNode);
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
    connectNode(*srcPort_p, *dstPort_p);
}

void SessionManager::disconnectNode(Port& src, Port& dst)
{
    QString err;
    src.owner.parentFactory()->disconnect(src, dst, &err);
    if (!err.isEmpty()) {
        emit operationFailed(err);
        return;
    }
    emit nodeDisconnected(&src.owner, &dst.owner);
}

void SessionManager::disconnectNode(int srcNode, int srcPort, int dstNode, int dstPort)
{
    AbstractNode* srcNode_p = m_activeFactory->nodes().value(srcNode);
    AbstractNode* dstNode_p = m_activeFactory->nodes().value(dstNode);

    if (!srcNode_p) {
        qWarning() << "Invalid Source Node";
        return;
    }
    if (!dstNode_p) {
        qWarning() << "Invalid or Destination Node";
        return;
    }

    Port* srcPort_p = srcNode_p->getPortFromIndex(srcPort);
    Port* dstPort_p = dstNode_p->getPortFromIndex(dstPort);
    if (!srcPort_p || !dstPort_p) {
        qWarning() << "Invalid Source or Destination Port";
        return;
    }
    disconnectNode(*srcPort_p, *dstPort_p);
}

// ---------- NODE SETTERS ---------------
void SessionManager::renameNode(int index, QString name)
{
    if (name.isEmpty()) {
        qWarning() << "Invalid Name: " << name;
        return;
    }

    auto nodes = m_activeFactory->nodes();
    if (index < 0 || index >= nodes.count()) {
        qWarning() << "Invalid Index: " << index;
        return;
    }

    AbstractNode* node = nodes.value(index);
    if (!node) {
        qWarning() << "Invalid Index: " << index;
        return;
    }
    node->setName(name);
    qDebug() << "Node renamed to " << name;
}

void SessionManager::setMachineLimit(AbstractNode* node, float limit)
{
    if (auto* mn = dynamic_cast<MachineNode*>(node)) {
        mn->setMachineLimit(limit);
        emit machineLimitChanged(mn);
    }
}

void SessionManager::setExtractionPurity(AbstractNode* node, NodePurity purity)
{
    if (auto* en = dynamic_cast<ExtractionNode*>(node)) {
        en->setPurity(purity);
        // emit machineLimitChanged(mn);

    }
}

void SessionManager::setExtractionTier(AbstractNode* node, int tier)
{
    if (auto* en = dynamic_cast<ExtractionNode*>(node))
        en->m_tier = tier;
}

// ----------------------------- SAVE AND LOAD ---------------------------------
void SessionManager::save(const QString& path)
{
    // TODO: add compression?
    // remove default and empty values
    QJsonObject root;
    root["metadata"] = QJsonObject { { "version", 1 } };

    QJsonArray factoriesJsonArray = QJsonArray();
    std::queue<Factory*> factoryQueue;
    factoryQueue.push(rootFactory());
    while (!factoryQueue.empty()) {
        Factory* f = factoryQueue.front();
        factoryQueue.pop();
        factoriesJsonArray.append(serializeFactory(f, factoryQueue));
    }
    root["factories"] = factoriesJsonArray;

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

    m_solver->reset();

    QJsonObject root = doc.object();
    std::queue<std::pair<Factory*, QJsonObject>> pendingFactories;
    // QMap<QUuid, AbstractNode*> uuidMap;
    QMap<QUuid, Factory*> factoryUuidMap;
    QJsonArray factoriesArray = root["factories"].toArray();
    QJsonObject rootFactoryObject = factoriesArray[0].toObject();
    Factory* newRoot = new Factory(nullptr, QString("Main"));
    newRoot->setParent(this);
    // rootFactory->setId(QUuid(factoriesArray[0].toObject()["id"].toString()));

    // ---------- CREATE FACTORIES ---------------
    for (auto fArr : factoriesArray) {
        QJsonObject fObject = fArr.toObject();
        QUuid id = QUuid(fObject["id"].toString());
        QString parentId = fObject["parentId"].toString();
        QString name = fObject["name"].toString();
        if (parentId.isEmpty()) {
            newRoot->setId(id);
            factoryUuidMap.insert(id, newRoot);
            continue;
        }
        Factory* parentF = factoryUuidMap.value(QUuid(parentId));
        Factory* f = parentF->createFactory(name, true);
        f->setId(id);
        factoryUuidMap.insert(id, f);
    }

    // ---------- CREATE NODES ---------------
    for (auto fArr : factoriesArray) {
        QJsonObject fObject = fArr.toObject();
        QUuid id = QUuid(fObject["id"].toString());
        Factory* f = factoryUuidMap.value(id);

        for (auto mArr : fObject["machines"].toArray()) {
            QJsonObject machineObject = mArr.toObject();
            AbstractNode* node = createNodeFromJson(machineObject, f, factoryUuidMap);
        }
        for (auto fNodeArr : fObject["factoryNodes"].toArray()) {
            QJsonObject factoryNodeObject = fNodeArr.toObject();
            AbstractNode* node = createNodeFromJson(factoryNodeObject, f, factoryUuidMap);
        }
        for (auto eArr : fObject["edges"].toArray()) {
            QJsonObject edgeObject = eArr.toObject();
            AbstractNode* node = createNodeFromJson(edgeObject, f, factoryUuidMap);
        }
    }

    // ---------- CREATE CONNECTIONS ---------------
    for (auto fArr : factoriesArray) {
        QJsonObject fObject = fArr.toObject();
        QUuid factoryId = QUuid(fObject["id"].toString());
        Factory* f = factoryUuidMap.value(factoryId);

        for (auto connArr : fObject["connections"].toArray()) {
            QJsonObject conn = connArr.toObject();
            int fromNodeIdx       = conn["from"].toArray()[0].toInt();
            int fromPortIdx       = conn["from"].toArray()[1].toInt();
            int toNodeIdx         = conn["to"].toArray()[0].toInt();
            int toPortIdx         = conn["to"].toArray()[1].toInt();
            connectNode(fromNodeIdx, fromPortIdx, toNodeIdx, toPortIdx, f);
        }
    }


    delete m_rootFactory;
    m_rootFactory = newRoot;
    m_activeFactory = m_rootFactory;
    emit factoryChanged(m_activeFactory);

    m_solver->onLoad();
}

QJsonObject SessionManager::serializeFactory(const Factory* factory, std::queue<Factory*>& factoryQueue)
{
    for (auto* subFactory : factory->subFactories())
        factoryQueue.push(subFactory);

    QJsonObject obj;
    obj["id"] = factory->id().toString();
    obj["name"] = factory->name();
    obj["parentId"] = factory->parent() ? factory->parent()->id().toString() : "";

    QJsonArray machinesArray, factoryNodesArray;
    for (auto* node : factory->nodes()) {
        switch (node->type()) {
        case NodeType::Production:
        case NodeType::Extraction:
            machinesArray.append(node->getJsonNode());
            break;
        case NodeType::Factory:
            factoryNodesArray.append(node->getJsonNode());
            break;
        default:
            break;
        }
    }
    obj["machines"] = machinesArray;
    obj["factoryNodes"] = factoryNodesArray;

    // ---------- edges ---------------
    QJsonArray edgesArray;
    for (auto* edge : factory->edges())
        edgesArray.append(edge->getJsonNode());
    obj["edges"] = edgesArray;

    // ---------- connections ---------------
    QJsonArray connections;
    for (auto conn : factory->connections())
        connections.append(conn->getJsonObject());
    obj["connections"] = connections;

    return obj;
}

AbstractNode* SessionManager::createNodeFromJson(QJsonObject j, Factory* f, QMap<QUuid, Factory*> map)
{
    NodeType type = static_cast<NodeType>(j["type"].toInt());
    float posX = j["posX"].toDouble();
    float posY = j["posY"].toDouble();
    QString name = j["name"].toString();
    QUuid id = QUuid(j["id"].toString());

    AbstractNode* node = nullptr;

    switch (type) {
    case NodeType::FactoryEdge: {
        PortType edgeType = static_cast<PortType>(j["edgeType"].toInt());
        node = f->createFactoryEdgeNode(edgeType);
        break;
    }
    case NodeType::Factory: {
        QUuid targetFactoryId = QUuid(j["factoryId"].toString());
        Factory* targetFactory = map.value(targetFactoryId);
        node = f->createFactoryNode(targetFactory, name);
        break;
    }
    case NodeType::Extraction: {
        float machineLimit = j["machineLimit"].toDouble();
        NodePurity purity = static_cast<NodePurity>(j["purity"].toInt());
        int tier = j["tier"].toInt();
        QString recipeName = j["recipe"].toString();
        const ExtractionRecipe* recipe = GameLibrary::get().getExtRecipeByClass(recipeName);
        ExtractionNode* en = f->createExtractionNode(*recipe, tier, name);
        en->setPurity(purity);
        en->setMachineLimit(machineLimit);
        node = en;
        break;
    }
    case NodeType::Production: {
        float machineLimit = j["machineLimit"].toDouble();
        QString recipeName = j["recipe"].toString();
        const ProductionRecipe* recipe = static_cast<const ProductionRecipe*>(GameLibrary::get().getRecipeByClass(recipeName));
        ProductionNode* pn = f->createProductionNode(*recipe, name);
        pn->setMachineLimit(machineLimit);
        node = pn;
        break;
    }
    default:
        return nullptr;
        break;
    }

    node->setId(id);
    node->setPosX(posX);
    node->setPosY(posY);
    return node;
}

void SessionManager::solve()
{
    // FIX: later we will need to seperate the build, and skip it
    m_solver->clear();
    m_solver->build(m_rootFactory);
    m_solver->solve();
    notifySolved();
}

void SessionManager::notifySolved()
{
    for (AbstractNode* node : m_activeFactory->nodes())
        node->notifySolved();
    emit solved();
}
