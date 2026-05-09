#pragma once

#include "core/managers/SessionManager.h"
#include "core/nodes/AbstractNode.h"
#include "gui/items/AbstractItem.h"
#include <QMap>
#include <QObject>

class ProductionItem;

class SceneManager : public QObject {
    Q_OBJECT;

private:
    QMap<AbstractNode*, AbstractItem*> m_nodeItemMap;
    SessionManager* m_session = nullptr;

public:
    explicit SceneManager(SessionManager* session, QObject* parent = nullptr);

    AbstractItem* itemFromNode(AbstractNode* node);

public slots:
    void onNodeAdded(AbstractNode* node);
    void onNodeRemoved(AbstractNode* node);

    ProductionItem* createProductionItem(const Recipe& recipe, Factory* factory = nullptr, QString name = QString(), QPointF pos = QPointF(0,0));
};
