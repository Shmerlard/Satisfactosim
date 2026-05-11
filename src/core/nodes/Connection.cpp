#include "Connection.h"
#include "core/nodes/Port.h"
#include "AbstractNode.h"

Connection::Connection(Port* src, Port* dst)
    : m_src(src)
    , m_dst(dst)
{
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
    obj["from"] = QJsonArray { m_src->owner.index(), m_src->owner.getPortIndex(*m_src) };
    obj["to"] = QJsonArray { m_dst->owner.index(), m_dst->owner.getPortIndex(*m_dst) };
    return obj;
}
