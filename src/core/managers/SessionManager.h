#pragma once

#include "core/nodes/ExtractionNode.h"
#include "core/nodes/Factory.h"
#include "core/nodes/FactoryEdgeNode.h"
#include "core/nodes/FactoryNode.h"
#include "core/nodes/ProductionNode.h"
#include "core/nodes/SplitterNode.h"
#include "core/types/Types.h"
#include "core/solvers/AbstractSolver.h"
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
        m_solver = AbstractSolver::create(AbstractSolver::SolverType::Gaussian, this);
        m_solver->onLoad();
    }
    Factory* m_rootFactory = nullptr;
    Factory* m_activeFactory = nullptr;
    AbstractSolver* m_solver = nullptr;

public:
    // ---------- GETTERS ---------------
    Factory* rootFactory() const { return m_rootFactory; }
    Factory* activeFactory() const { return m_activeFactory; }

    // ---------- SOLVER ---------------
    void setSolverType(AbstractSolver::SolverType type)
    {
        m_solver = AbstractSolver::create(type, this);
        m_solver->onLoad();
    }

    // ---------- NODE CREATION ---------------
    FactoryEdgeNode* createFactoryEdgeNode(PortType edgeType, Factory* factory = nullptr, QString name = QString());
    FactoryNode* createFactoryNode(Factory& parent, Factory& owned, QString name = QString());
    ProductionNode* createProductionNode(const ProductionRecipe& recipe, Factory* factory = nullptr, QString name = QString());
    ProductionNode* createProductionNodeByClass(const QString& rClass, Factory* factory = nullptr, QString name = QString());
    ExtractionNode* createExtractionNode(const ExtractionRecipe& recipe, int tier = 0, Factory* factory = nullptr, QString name = QString());
    ExtractionNode* createExtractionNodeByName(QString resourceName, int tier = 0, Factory* factory = nullptr, QString name = QString());
    // ExtractionNode* createExtractionNodeByClass(QString rClass, int tier = 1, Factory* factory = nullptr, QString name = QString());
    SplitterNode* createSplitterNode(QList<Frac> weights = {Frac(1), Frac(1)}, Factory* factory = nullptr, QString name = QString());
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
    void incMachineLimit(AbstractNode* node, int limit);
    void setMachineSomersloop(MachineNode* node, int count);
    void setExtractionPurity(AbstractNode* node, NodePurity purity);
    void setExtractionTier(AbstractNode* node, int tier);
    void solve();
    void notifySolved();

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
    void machineLimitChanged(MachineNode* node);
    void extractionTierChanged(ExtractionNode* node);
    void extractionPurityChanged(ExtractionNode* node);
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
