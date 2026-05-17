#include "Connection.h"
#include "AbstractNode.h"
#include "core/nodes/Port.h"

Connection::Connection(Port* src, Port* dst)
{
    if (src->type == PortType::Output) {
        m_src = src;
        m_dst = dst;
    } else {
        m_src = dst;
        m_dst = src;
    }
}

void Connection::addMidPoint(int index)
{
}
void Connection::removeMidPoint(int index)
{
}

Port* Connection::getPeer(Port& port)
{
    if (m_dst == &port)
        return m_src;
    if (m_src == &port)
        return m_dst;
    return nullptr;
}

QJsonObject Connection::getJsonObject()
{
    QJsonObject obj;
    // obj["from"] = QJsonArray { m_src->owner.index(), m_src->item ? m_src->item->itemClass : "" };
    // obj["to"]   = QJsonArray { m_dst->owner.index(), m_dst->item ? m_dst->item->itemClass : "" };
    obj["from"] = QJsonArray { m_src->owner.index(), m_src->owner.getPortIndex(*m_src) };
    obj["to"] = QJsonArray { m_dst->owner.index(), m_dst->owner.getPortIndex(*m_dst) };
    return obj;
}
//
QObject* Connection::srcNodeObj() const
{
    return &m_src->owner;
}

QObject* Connection::dstNodeObj() const
{
    return &m_dst->owner;
}

int Connection::srcPortIdx() const
{
    return m_src->owner.getPortIndex(*m_src, false);
}

int Connection::dstPortIdx() const
{
    return m_dst->owner.getPortIndex(*m_dst, false);
}

QPointF Connection::srcOffset()
{
    return m_src->offset;
}
QPointF Connection::dstOffset()
{
    return m_dst->offset;
}
void Connection::setSrcOffset(QPointF offset)
{
    m_src->offset = offset;
    emit offsetChanged();
}
void Connection::setDstOffset(QPointF offset)
{
    m_dst->offset = offset;
    emit offsetChanged();
}
