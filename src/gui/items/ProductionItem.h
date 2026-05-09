#pragma once

#include "AbstractItem.h"
#include "core/nodes/ProductionNode.h"

class ProductionItem : public AbstractItem {
    Q_OBJECT;

private:
public:
    explicit ProductionItem(AbstractNode* node, QPointF pos, QObject* parent = nullptr);
    QString machineName() const;
};
