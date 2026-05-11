#pragma once

#include "core/nodes/ExtractionNode.h"
#include "core/nodes/Factory.h"
#include "core/nodes/FactoryNode.h"
#include "core/nodes/ProductionNode.h"
#include "core/nodes/FactoryEdgeNode.h"
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
        ProductionNode* t1 = createProductionNodeByClass("Recipe_IngotIron_C", m_activeFactory, "RRA");
        ProductionNode* t2 = createProductionNodeByClass("Recipe_IronPlate_C");

    }
    Factory* m_rootFactory = nullptr;
    Factory* m_activeFactory = nullptr;

signals:
    void operationFailed(const QString& reason);
    void nodeAdded(AbstractNode* node);
    void nodeConnected();
    void nodeDisconnected();
    void portsConnected(AbstractNode* srcNode, int srcPortIdx, AbstractNode* dstNode, int dstPortIdx);
    void nodeRemoved(AbstractNode* node);
    void factoryChanged(Factory* factory);
    void solved();

public:
    FactoryNode* createFactoryNode(Factory& parent, Factory& owned, QString name = QString());

    bool deserializeFactory(
            std::pair<Factory*, QJsonObject>& currentFactory,
            std::queue<std::pair<Factory*, QJsonObject>>& pendingFactories);
public slots:
    void save(const QString& path);
    void load(const QString& path);

    FactoryEdgeNode* createFactoryEdgeNode(PortType edgeType, Factory* factory = nullptr, QString name = QString());
    ProductionNode* createProductionNode(const ProductionRecipe& recipe, Factory* factory = nullptr, QString name = QString());
    ProductionNode* createProductionNodeByClass(const QString& rClass, Factory* factory = nullptr, QString name = QString());
    ExtractionNode* createExtractionNode(const ExtractionRecipe* recipe, int tier = 1, Factory* factory = nullptr, QString name = QString());
    ExtractionNode* createExtractionNodeByName(QString resourceName, int tier = 1, Factory* factory = nullptr, QString name = QString());
    ExtractionNode* createExtractionNodeByClass(QString rClass, int tier = 1, Factory* factory = nullptr, QString name = QString());

    Factory* createFactory(Factory* parent = nullptr, QString name = QString());
    void enterFactory(Factory* f);
    void enterRootFactory();
    void enterParentFactory();

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
