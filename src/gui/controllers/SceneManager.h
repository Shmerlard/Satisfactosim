#pragma once

#include "RecipeListModel.h"
#include "SceneModel.h"
#include "core/managers/SessionManager.h"
#include "core/nodes/AbstractNode.h"

#include <QMap>
#include <QObject>
#include <QQmlEngine>
#include <QVariantList>

class SceneManager : public QObject {
    Q_OBJECT;
    QML_ELEMENT;
    QML_SINGLETON;

    Q_PROPERTY(SceneModel* model READ model CONSTANT);
    Q_PROPERTY(RecipeFilterModel* recipes READ recipes CONSTANT);
    Q_PROPERTY(bool isRootFactory READ isRootFactory NOTIFY factoryChanged);
    Q_PROPERTY(QVariantList connections READ connections NOTIFY connectionsChanged);

private:
    SessionManager* m_session = nullptr;
    SceneModel* m_model = nullptr;
    RecipeListModel* m_recipeModel = nullptr;
    RecipeFilterModel* m_recipeFilterModel = nullptr;
    QVariantList m_connections;

    explicit SceneManager(QObject* parent = nullptr);

public:
    static SceneManager& get()
    {
        static SceneManager inst;
        return inst;
    }
    static SceneManager* create(QQmlEngine* engine, QJSEngine*)
    {
        engine->setObjectOwnership(&get(), QQmlEngine::CppOwnership);
        return &get();
    }
    SceneModel* model() const { return m_model; }
    RecipeFilterModel* recipes() const { return m_recipeFilterModel; }
    bool isRootFactory() const { return m_session->activeFactory() == m_session->rootFactory(); }
    QVariantList connections() const { return m_connections; }

signals:
    void factoryChanged();
    void connectionsChanged();

private:
    void loadConnections(Factory* f);

public slots:
    void onNodeAdded(AbstractNode* node);
    void onNodeRemoved(AbstractNode* node);

    void enterFactory(AbstractNode* factoryNode);
    void enterParentFactory();
    void enterRootFactory();
    // void createMachineNode(Recipe* recipe, double x, double y);
    AbstractNode* createMachineNode(const QString recipe, double x, double y);
    void createAndConnectMachineNode(const QString recipe, double x, double y, int srcNodeIdx, int srcPortIdx);
    void createSubFactory(const QString name, double x, double y);
    void createEdgeNode(bool isInput, const QString name, double x, double y);
    void deleteNode(AbstractNode* node);
    void deleteConnection(QObject* connObj);
    void setPortOffset(int nodeIndex, int portIndex, QPointF offset);
    void connectNodes(int srcNodeIdx, int srcPortIdx, int dstNodeIdx, int dstPortIdx);
    void setMachineLimit(AbstractNode* node, float limit);
    void incMachineLimit(AbstractNode* node, int inc);
    void solve();
    // QVariantMap portAtPosition(qreal x, qreal y, qreal tolerance = 15.0);
};
