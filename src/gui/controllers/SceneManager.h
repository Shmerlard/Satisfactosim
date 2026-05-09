#pragma once

#include "core/managers/SessionManager.h"
#include "core/nodes/AbstractNode.h"
#include "SceneModel.h"

#include <QMap>
#include <QObject>

class SceneManager : public QObject {
    Q_OBJECT;

    Q_PROPERTY(SceneModel* model READ model CONSTANT)

private:
    SessionManager* m_session = nullptr;
    SceneModel* m_model = nullptr;

public:
    explicit SceneManager(SessionManager* session, QObject* parent = nullptr);
    SceneModel* model() const { return m_model; }

public slots:
    void onNodeAdded(AbstractNode* node);
    void onNodeRemoved(AbstractNode* node);
};
