#pragma once

#include "core/nodes/ExtractionNode.h"
#include "core/nodes/Factory.h"
#include "core/nodes/FactoryNode.h"
#include "core/nodes/ProductionNode.h"
#include "core/nodes/FactoryEdgeNode.h"
#include "core/types/Types.h"
// #include "Solver.h"
// #include "gui/models/SceneModel.h"
#include <QObject>
#include <QQmlEngine>

class SessionManager : public QObject {
    Q_OBJECT;
    QML_SINGLETON;
    QML_ELEMENT;

    // Q_PROPERTY(Factory* rootFactory   READ rootFactory   CONSTANT)
    // Q_PROPERTY(Factory* activeFactory READ activeFactory NOTIFY activeFactoryChanged)
    // Q_PROPERTY(SceneModel* activeModel READ activeModel  CONSTANT)

private:
    explicit SessionManager(QObject* parent = nullptr)
        : QObject(parent)
    {
        m_rootFactory = new Factory(nullptr, QString("Main Factory"));
        m_activeFactory = m_rootFactory;
        ProductionNode* t1 = createProductionNodeByClass("Recipe_IngotIron_C");
        ProductionNode* t2 = createProductionNodeByClass("Recipe_IronPlate_C");

        // m_sceneModel = new SceneModel(this, this);
        // connect(this, &SessionManager::nodeAdded, m_sceneModel, &SceneModel::onNodeAdded);
        // m_sceneModel->loadFromFactory(m_rootFactory);
    }
    // FIX: std::uniquepointer
    Factory* m_rootFactory = nullptr;
    Factory* m_activeFactory = nullptr;
    // SceneModel* m_sceneModel = nullptr;

signals:
    void operationFailed(const QString& reason);
    void nodeAdded(AbstractNode* node);
    void nodeConnected();
    void nodeDisconnected();
    void portsConnected(AbstractNode* srcNode, int srcPortIdx, AbstractNode* dstNode, int dstPortIdx);
    void nodeRemoved(AbstractNode* node);
    void factoryChanged(Factory* factory);
    void activeFactoryChanged();
    void solved();

public:
    FactoryNode* createFactoryNode(Factory& parent, Factory& owned, QString name = QString());

public slots:
    // void goParentFactory();
    // void goToRoot();
    void save(const QString& path);
    void load(const QString& path);

    FactoryEdgeNode* createFactoryEdgeNode(PortType edgeType, Factory* factory = nullptr, QString name = QString());
    ProductionNode* createProductionNode(const Recipe& recipe, Factory* factory = nullptr, QString name = QString());
    // ProductionNode* createProductionNode(const QString& recipe, Factory* factory = nullptr, QString name = QString());
    ProductionNode* createProductionNodeByClass(const QString& rClass, Factory* factory = nullptr, QString name = QString());
    ExtractionNode* createExtractionNode(const ExtractionRecipe* recipe, int tier = 1, Factory* factory = nullptr, QString name = QString());
    ExtractionNode* createExtractionNodeByName(QString resourceName, int tier = 1, Factory* factory = nullptr, QString name = QString());
    ExtractionNode* createExtractionNodeByClass(QString rClass, int tier = 1, Factory* factory = nullptr, QString name = QString());

    Factory* createFactory(Factory* parent = nullptr, QString name = QString());
    void enterFactory(Factory* f);

    // FactoryEdgeNode* createFactoryEdgeNode(PortType portType, Factory* parentFactory = nullptr, QString name = QString());
    //
    void connectNode(Port* src, Port* dest);
    void connectNode(int srcNode, int srcPort, int dstNode, int dstPort);
    void disconnectNode(Port* src, Port* dest);
    void disconnectNode(int srcNode, int srcPort, int dstNode, int dstPort);

    void renameNode(int index, QString name);
    void deleteNode(AbstractNode* node);
    void setMachineLimit(AbstractNode* node, float limit);
    void setExtractionPurity(AbstractNode* node, NodePurity purity);
    void setExtractionTier(AbstractNode* node, int tier);
    // Solver::Result solve();

    Factory* rootFactory() const { return m_rootFactory; }
    Factory* activeFactory() const { return m_activeFactory; }
    // SceneModel* activeModel()   const { return m_sceneModel; }

    // QList<Factory*> getSubFactories() const { return m_activeFactory->subFactories(); };

public:
    static SessionManager& get()
    {
        static SessionManager inst;
        return inst;
    }

    static SessionManager* create(QQmlEngine*, QJSEngine*)
    {
        return &get();
    }
};
