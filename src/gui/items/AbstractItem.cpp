
#include "AbstractItem.h"

AbstractItem::AbstractItem(AbstractNode* node, QPointF pos, QObject* parent)
    : QObject(parent)
    , m_node(node)
    , m_pos(pos)
{
}

QString AbstractItem::name() const
{
    return m_node ? m_node->name() : QString("unnamed");
}
