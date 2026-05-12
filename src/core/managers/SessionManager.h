#pragma once

#include "core/nodes/ExtractionNode.h"
#include "core/nodes/Factory.h"
#include "core/nodes/FactoryEdgeNode.h"
#include "core/nodes/FactoryNode.h"
#include "core/nodes/ProductionNode.h"
#include "core/types/Types.h"
// #include "Solver.h"
#include <QObject>
#include <QQmlEngine>
#include <queue>

class SessionManager : public QObject {
    Q_OBJECT;
    QML_SINGLETON;
    QML_ELEMENT;

private:
    explicit SessionManager(QObject* parent = nullptr)
        : QObject(parent)
    {
        m_rootFactory = new Factory(nullptr, QString("Main Factory"));
        m_rootFactory->setParent(this);
        m_activeFactory = m_rootFactory;
    }
    Factory* m_rootFactory = nullptr;
    Factory* m_activeFactory = nullptr;

public:
    // ---------- GETTERS ---------------
    Factory* rootFactory() const { return m_rootFactory; }
    Factory* activeFactory() const { return m_activeFactory; }

public:
    // ---------- NODE CREATION ---------------
    FactoryEdgeNode* createFactoryEdgeNode(PortType edgeType, Factory* factory = nullptr, QString name = QString());
    FactoryNode* createFactoryNode(Factory& parent, Factory& owned, QString name = QString());
    ProductionNode* createProductionNode(const ProductionRecipe& recipe, Factory* factory = nullptr, QString name = QString());
    ProductionNode* createProductionNodeByClass(const QString& rClass, Factory* factory = nullptr, QString name = QString());
    ExtractionNode* createExtractionNode(const ExtractionRecipe& recipe, int tier = 1, Factory* factory = nullptr, QString name = QString());
    ExtractionNode* createExtractionNodeByName(QString resourceName, int tier = 1, Factory* factory = nullptr, QString name = QString());
    // ExtractionNode* createExtractionNodeByClass(QString rClass, int tier = 1, Factory* factory = nullptr, QString name = QString());
    void deleteNode(AbstractNode* node);

    // ---------- FACTORY MANIPULATION ---------------
    Factory* createFactory(Factory* parent = nullptr, QString name = QString());
    void enterFactory(Factory* f);
    void enterRootFactory();
    void enterParentFactory();

    // ---------- CONNECTIONS ---------------
    void connectNode(Port& src, Port& dest);
    void connectNode(int srcNode, int srcPort, int dstNode, int dstPort, Factory* f = nullptr);
    void disconnectNode(Port& src, Port& dest);
    void disconnectNode(int srcNode, int srcPort, int dstNode, int dstPort);

    // ---------- NODE SETTERS ---------------
    void renameNode(int index, QString name);
    void setMachineLimit(AbstractNode* node, float limit);
    void setExtractionPurity(AbstractNode* node, NodePurity purity);
    void setExtractionTier(AbstractNode* node, int tier);
    // Solver::Result solve();

    // ---------- SAVE AND LOAD ---------------
    void save(const QString& path);
    void load(const QString& path);
    QJsonObject serializeFactory(const Factory* factory, std::queue<Factory*>& factoryQueue);
    AbstractNode* createNodeFromJson(QJsonObject j, Factory* f, QMap<QUuid, Factory*> map);
    bool deserializeFactory(
        std::pair<Factory*, QJsonObject>& currentFactory,
        std::queue<std::pair<Factory*, QJsonObject>>& pendingFactories);

signals:
    void operationFailed(const QString& reason);
    void nodeAdded(AbstractNode* node);
    void nodeRemoved(AbstractNode* node);
    void nodeConnected(AbstractNode* src, AbstractNode* dst);
    void nodeDisconnected(AbstractNode* src, AbstractNode* dst);
    void factoryChanged(Factory* factory);
    void solved();

public:
    static SessionManager& get()
    {
        static SessionManager inst;
        return inst;
    }

    static SessionManager* create(QQmlEngine* engine, QJSEngine*)
    {
        engine->setObjectOwnership(&get(), QQmlEngine::CppOwnership);
        return &get();
    }
};
