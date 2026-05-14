#include "AbstractNode.h"
#include "Connection.h"
#include "Factory.h"

AbstractNode::AbstractNode(
    Factory& parentFactory,
    QString name,
    QUuid id)
    : QObject(&parentFactory)
    , m_parentFactory(&parentFactory)
    , m_id(id.isNull() ? QUuid::createUuid() : id)
    , m_name(name)
{
}

// ---------- MISC ---------------
QJsonObject AbstractNode::getJsonNode() const
{

    QJsonObject obj;
    obj["id"] = m_id.toString();
    obj["name"] = m_name;
    obj["type"] = static_cast<uint8_t>(m_type);
    obj["posX"] = posX();
    obj["posY"] = posY();
    return obj;
}

int AbstractNode::index() const
{
    // FIX: may need a const_cast
    // return m_parentFactory->subNodes().indexOf(this);
    int idx = 0;
    for (auto& it : m_parentFactory->nodes()) {
        if (this == it)
            return idx;
        idx++;
    }
    return -1;
}

// ---------- PORTS ---------------
void AbstractNode::deletePorts()
{
    m_outputs.clear();
    m_inputs.clear();
}

void AbstractNode::disconnectAllPorts()
{
    for (auto& port : inputs())
        port->disconnect();
    for (auto& port : outputs())
        port->disconnect();
}

int AbstractNode::getPortIndex(Port& port, bool isOffset) const
{
    for (int i = 0; i < m_inputs.size(); i++)
        if (m_inputs[i].get() == &port)
            return i;

    for (int i = 0; i < m_outputs.size(); i++)
        if (m_outputs[i].get() == &port)
            return i + isOffset * m_inputs.size();

    return -1;
}

Port* AbstractNode::getPortFromIndex(int index) const
{
    if (index < 0)
        return nullptr;
    int cnt_in = m_inputs.size();
    int cnt_total = cnt_in + m_outputs.size();

    if (index < cnt_in)
        return m_inputs[index].get();
    else if (index < cnt_total)
        return m_outputs[index - cnt_in].get();

    return nullptr;
}

QVariantList AbstractNode::inputsQml() const
{
    QVariantList list;
    int idx = 0;
    for (const auto& p : m_inputs) {
        QVariantMap map;
        map["iconUrl"] = p->item ? "image://assets/item/" + p->item->itemClass : "";
        map["amount"] = p->amount;
        map["portIndex"] = idx++;
        map["nodeIndex"] = index();
        list.append(map);
    }
    return list;
}

QVariantList AbstractNode::outputsQml() const
{
    QVariantList list;
    int idx = static_cast<int>(m_inputs.size());
    for (const auto& p : m_outputs) {
        QVariantMap map;
        map["iconUrl"] = p->item ? "image://assets/item/" + p->item->itemClass : "";
        map["amount"] = p->amount;
        map["portIndex"] = idx++;
        map["nodeIndex"] = index();
        list.append(map);
    }
    return list;
}

// ---------- SETTERS ---------------
void AbstractNode::setName(QString name)
{
    m_name = name;
    emit nameChanged();
}

void AbstractNode::setPos(QPointF pos)
{
    m_pos = pos;
    emit posChanged();
}

void AbstractNode::setPosX(double x)
{
    m_pos.setX(x);
    emit posChanged();
}

void AbstractNode::setPosY(double y)
{
    m_pos.setY(y);
    emit posChanged();
}
