#pragma once

#include "core/nodes/Factory.h"
#include "core/nodes/FactoryNode.h"
#include "core/nodes/ProductionNode.h"
#include "core/nodes/ExtractionNode.h"
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
        // m_sceneModel = new SceneModel(this, this);
        // connect(this, &SessionManager::nodeAdded, m_sceneModel, &SceneModel::onNodeAdded);
        // m_sceneModel->loadFromFactory(m_rootFactory);
    }
    Factory* m_rootFactory = nullptr;
    Factory* m_activeFactory = nullptr;
    // SceneModel* m_sceneModel = nullptr;

signals:
    void nodeAdded(AbstractNode* node);
    void nodeConnected();
    void portsConnected(AbstractNode* srcNode, int srcPortIdx, AbstractNode* dstNode, int dstPortIdx);
    void nodeRemoved(AbstractNode* node);
    void factoryChanged(Factory* factory);
    void activeFactoryChanged();
    void solved();

public slots:
    // void goParentFactory();
    // void goToRoot();
    // void save(const QString& path);
    // void load(const QString& path);

    ProductionNode* createProductionNode(const Recipe& recipe, Factory* factory = nullptr, QString name = QString());
    ProductionNode* createProductionNodeByClass(const QString& rClass, Factory* factory = nullptr, QString name = QString());
    ExtractionNode* createExtractionNode(const ExtractionRecipe* recipe, int tier = 1, Factory* factory = nullptr, QString name = QString());
    ExtractionNode* createExtractionNodeByName(QString resourceName, int tier = 1, Factory* factory = nullptr, QString name = QString());

    Factory* createFactory(Factory* parent = nullptr, QString name = QString());
    void enterFactory(Factory* f);
    //
    // void connectNode(Port* src, Port* dest);
    // void connectNode(int srcNode, int srcPort, int dstNode, int dstPort);
    // void deleteNode(AbstractNode* node);
    // void setMachineLimit(AbstractNode* node, float limit);
    // void setExtractionPurity(AbstractNode* node, NodePurity purity);
    // void setExtractionTier(AbstractNode* node, int tier);
    // Solver::Result solve();

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

    Factory* rootFactory() const { return m_rootFactory; }
    Factory* activeFactory() const { return m_activeFactory; }
    // SceneModel* activeModel()   const { return m_sceneModel; }

    QList<Factory*> getSubFactories() const { return m_activeFactory->subFactories(); };

};
