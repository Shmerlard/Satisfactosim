#include "SplitterNode.h"

SplitterNode::SplitterNode(
    Factory& parentFactory,
    QList<Frac> weights,
    QString name,
    QUuid id)
    : AbstractNode(parentFactory, name, id)
{
    m_type = NodeType::Splitter;
    for (Frac weight : weights)
        addOutput(weight);

    m_inputs.push_back(std::make_unique<Port>(*this, nullptr, PortType::Input));
}

void SplitterNode::addOutput(Frac weight)
{
    m_outputs.push_back(std::make_unique<Port>(*this, nullptr, PortType::Output));
    m_weightMap[m_outputs.back().get()] = weight;
    m_total += weight;
}

Frac SplitterNode::proportion(Port& port)
{
    Frac w = m_weightMap.value(&port, Frac(-1));
    if (w == Frac(-1))
        return w;
    return w / m_total;
}

void SplitterNode::setWeight(Port& port, Frac weight)
{
    if (m_weightMap.contains(&port))
        m_weightMap[&port] = weight;
}

// FIX: implement proper portRate for splitter (proportional weight-based flow)
Frac SplitterNode::portRate(const Port* port) const
{
    return Frac(0);
}
