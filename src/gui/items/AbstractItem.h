#pragma once

#include "core/nodes/AbstractNode.h"
#include <QObject>
#include <QPointF>

class AbstractItem : public QObject {
    Q_OBJECT;

    Q_PROPERTY(QString name READ name CONSTANT);

friend class SceneManager;
protected:
    explicit AbstractItem(AbstractNode* node, QPointF pos, QObject* parent = nullptr);
    AbstractNode* m_node = nullptr;
    QPointF m_pos = QPointF(0, 0);

public:
    QString name() const;
};
