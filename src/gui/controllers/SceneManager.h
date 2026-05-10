#pragma once

#include "core/managers/SessionManager.h"
#include "core/nodes/AbstractNode.h"
#include "SceneModel.h"

#include <QMap>
#include <QObject>

class RecipeListModel;
class SceneManager : public QObject {
    Q_OBJECT;

    Q_PROPERTY(SceneModel* model READ model CONSTANT);
    Q_PROPERTY(RecipeListModel* recipes READ recipes CONSTANT);
    Q_PROPERTY(bool isRootFactory READ isRootFactory NOTIFY factoryChanged)

private:
    SessionManager* m_session = nullptr;
    SceneModel* m_model = nullptr;
    RecipeListModel* m_recipeModel = nullptr;

public:
    explicit SceneManager(SessionManager* session, QObject* parent = nullptr);
    SceneModel* model() const { return m_model; }
    RecipeListModel* recipes() const { return m_recipeModel; }
    bool isRootFactory() const { return m_session->activeFactory() == m_session->rootFactory(); }

signals:
    void factoryChanged();

public slots:
    void onNodeAdded(AbstractNode* node);
    void onNodeRemoved(AbstractNode* node);

    void enterFactory(AbstractNode* factoryNode);
};
