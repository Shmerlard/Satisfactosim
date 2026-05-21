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
    if (!port || &port->owner != this)
        return Frac(0);

    // Port* input

    if (port->type == PortType::Input)
        return Frac(0); // FIX: implement

    // const auto& list = (port->type == PortType::Input) ? recipe()->inputs : recipe()->outputs;
    // auto it = std::find_if(list.begin(), list.end(), [&](const auto& p) { return p.first == port->item; });
    return Frac(0);
}

QJsonObject SplitterNode::getJsonNode() const
{
    QJsonObject obj = AbstractNode::getJsonNode();
    // FIX: implement weights
    return obj;
}

void SplitterNode::onPortConnected(Port& port)
{
    if (port.item) {
        for (auto& p : m_inputs)
            if (!p->item)
                p->item = port.item;
        for (auto& p : m_outputs)
            if (!p->item)
                p->item = port.item;
    }
    emit portsChanged();
}

void SplitterNode::onPortDisconnected(Port& port)
{
    if (this->getNeighbors().isEmpty()) {
        for (auto& p : m_inputs) {
            p->item = nullptr;
            p->amount = 0.0f;
        }
        for (auto& p : m_outputs) {
            p->item = nullptr;
            p->amount = 0.0f;
        }
    }
    emit portsChanged();
}
