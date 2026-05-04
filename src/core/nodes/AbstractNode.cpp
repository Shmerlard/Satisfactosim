#include "AbstractNode.h"
#include "Factory.h"


AbstractNode::AbstractNode(
    Factory& parentFactory,
    QString name)
    : m_parentFactory(&parentFactory)
    , m_id(QUuid::createUuid())
    , m_name(name)
{
}

int AbstractNode::index() const
{
    // FIX: may need a const_cast
    return m_parentFactory->subNodes().indexOf(this);
}

int AbstractNode::getPortIndex(Port& port) const
{
    // FIX: we may not need to put all the ports in one line
    int inputIndex = m_inputs.indexOf(&port);
    if (inputIndex != -1)
        return inputIndex;

    int outputIndex = m_outputs.indexOf(&port);
    if (outputIndex != -1)
        return m_inputs.count() + outputIndex;

    return -1;
}

Port* AbstractNode::getPortFromIndex(int index) const
{
    if (index < 0)
        return nullptr;
    int cnt_in = m_inputs.count();
    int cnt_total = cnt_in + m_outputs.count();

    if (index < cnt_in)
        return m_inputs[index];
    else if (index < cnt_total)
        return m_outputs[index - cnt_in];

    return nullptr;
}

bool AbstractNode::connectToPort(Port& src, Port& dst, QString* err)
{
    QString _dummy;
    if (!err) err= &_dummy;
    if (&dst.owner == &src.owner){
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

    dst.connectedTo.append(&src);
    src.connectedTo.append(&dst);
    err->clear();
    return true;
}

void AbstractNode::deletePorts() {
    disconnectAllPorts();
    qDeleteAll(m_inputs);
    qDeleteAll(m_outputs);
    m_outputs.clear();
    m_inputs.clear();
}

void AbstractNode::disconnectPort(Port* port) {
    if (&port->owner != this) {
        qWarning() << "port owner doesnt match!";
        return;
    }
    port->amount = 0;
    for (auto* con : port->connectedTo) {
        con->connectedTo.removeAll(port);
    }
    port->connectedTo.clear();
}

void AbstractNode::disconnectAllPorts()
{
    for (auto* port : inputs()) {
        disconnectPort(port);
    }
    for (auto* port : outputs()) {
        disconnectPort(port);
    }
}

