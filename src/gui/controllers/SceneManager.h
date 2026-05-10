#pragma once

#include "core/managers/SessionManager.h"
#include "core/nodes/AbstractNode.h"
#include "RecipeListModel.h"
#include "SceneModel.h"

#include <QMap>
#include <QObject>

class SceneManager : public QObject {
    Q_OBJECT;

    Q_PROPERTY(SceneModel* model READ model CONSTANT);
    Q_PROPERTY(RecipeFilterModel* recipes READ recipes CONSTANT);
    Q_PROPERTY(bool isRootFactory READ isRootFactory NOTIFY factoryChanged)

private:
    SessionManager* m_session = nullptr;
    SceneModel* m_model = nullptr;
    RecipeListModel* m_recipeModel = nullptr;
    RecipeFilterModel* m_recipeFilterModel = nullptr;

public:
    explicit SceneManager(SessionManager* session, QObject* parent = nullptr);
    SceneModel* model() const { return m_model; }
    RecipeFilterModel* recipes() const { return m_recipeFilterModel; }
    bool isRootFactory() const { return m_session->activeFactory() == m_session->rootFactory(); }

signals:
    void factoryChanged();

public slots:
    void onNodeAdded(AbstractNode* node);
    void onNodeRemoved(AbstractNode* node);

    void enterFactory(AbstractNode* factoryNode);

    // void createMachineNode(Recipe* recipe, double x, double y);
    void createMachineNode(const QString recipe, double x, double y);
};
