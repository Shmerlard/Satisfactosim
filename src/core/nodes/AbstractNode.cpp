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

bool AbstractNode::connectToPort(Port& src, Port& dst)
{
    if (&dst.owner == &src.owner)
        return false;
    if (dst.type == src.type)
        return false;
    if (dst.connectedTo.contains(&src) || src.connectedTo.contains(&dst))
        return false;
    if (src.item && dst.item && src.item != dst.item)
        return false;
    // TODO: see what happens when one of them is false
    dst.connectedTo.append(&src);
    src.connectedTo.append(&dst);
    // call update
    return true;
}
