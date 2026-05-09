#include "ProductionItem.h"

ProductionItem::ProductionItem(AbstractNode* node, QPointF pos, QObject* parent)
    : AbstractItem(node, pos, parent)
{
}

QString ProductionItem::machineName() const
{
    ProductionNode* node = static_cast<ProductionNode*>(m_node);
    if (!node)
        return QString();
    if (!node->machine())
        return QString();

    return node->machine()->machineName;
}
