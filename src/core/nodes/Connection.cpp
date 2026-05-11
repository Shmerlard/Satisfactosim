#include "Connection.h"
#include "core/nodes/Port.h"

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
    return QJsonObject();
}
