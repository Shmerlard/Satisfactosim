#include "AbstractNode.h"
#include "Factory.h"

AbstractNode::AbstractNode(
    Factory& parentFactory,
    QString name,
    QUuid id)
    : m_parentFactory(&parentFactory)
    , m_id(id.isNull() ? QUuid::createUuid() : id)
    , m_name(name)
{
}

int AbstractNode::index() const
{
    // FIX: may need a const_cast
    // return m_parentFactory->subNodes().indexOf(this);
    int idx = 0;
    for (auto& it : m_parentFactory->subNodes()) {
        if (this == it.get())
            return idx;
        idx++;
    }
    return -1;
}

int AbstractNode::getPortIndex(Port& port) const
{
    // FIX: we may not need to put all the ports in one line
    for (int i = 0; i < m_inputs.size(); i++)
        if (m_inputs[i].get() == &port)
            return i;

    for (int i = 0; i < m_outputs.size(); i++)
        if (m_inputs[i].get() == &port)
            return i + m_inputs.size();
    // int inputIndex = m_inputs.indexOf(&port);
    // if (inputIndex != -1)
    //     return inputIndex;
    //
    // int outputIndex = m_outputs.indexOf(&port);
    // if (outputIndex != -1)
    //     return m_inputs.count() + outputIndex;

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

// TODO: look for differences between signals for notifying erros in session
// and *err
bool AbstractNode::connectToPort(Port& src, Port& dst, QString* err)
{
    QString _dummy;
    if (!err)
        err = &_dummy;
    if (&dst.owner == &src.owner) {
        *err = "source and destination belong to the same node!";
        return false;
    }
    if (dst.type == src.type) {
        *err = "source and destination are the same type!";
        return false;
    }
    if (dst.connectedTo.contains(&src) || src.connectedTo.contains(&dst)) {
        *err = "source and destination are already connected!";
        return false;
    }
    if (src.item && dst.item && src.item != dst.item) {
        *err = "cannot connect two different items ports";
        return false;
    }
    src.connect(dst);
    err->clear();
    return true;
}

QJsonObject AbstractNode::getJsonNode() const
{

    QJsonObject obj;
    obj["id"] = m_id.toString();
    obj["name"] = m_name;
    obj["type"] = static_cast<uint8_t>(m_type);
    return obj;
}

void AbstractNode::deletePorts()
{
    // disconnectAllPorts();
    // qDeleteAll(m_inputs);
    // qDeleteAll(m_outputs);
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
