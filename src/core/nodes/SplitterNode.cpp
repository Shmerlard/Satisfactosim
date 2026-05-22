#include "SplitterNode.h"
#include "core/types/FracUtils.h"
#include <QJsonArray>
#include <QJsonObject>

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

QVariantList SplitterNode::outputsQml() const
{
    QVariantList list = AbstractNode::outputsQml();
    int i = 0;
    for (const auto& p : m_outputs) {
        Frac w = m_weightMap.value(p.get(), Frac(1));
        Frac prop = m_total > Frac(0) ? w / m_total : Frac(0);
        QVariantMap map = list[i].toMap();
        map["weightN"] = w.numerator();
        map["weightD"] = w.denominator();
        map["weight"] = StringFromFrac(w);
        map["proportion"] = StringFromFrac(prop);
        list[i] = map;
        ++i;
    }
    return list;
}

void SplitterNode::addOutput(Frac weight)
{
    m_outputs.push_back(std::make_unique<Port>(*this, nullptr, PortType::Output));
    m_weightMap[m_outputs.back().get()] = weight;
    m_total += weight;
}

Frac SplitterNode::proportion(Port& port) const
{
    Frac w = m_weightMap.value(&port, Frac(-1));
    if (w == Frac(-1) || m_total == Frac(0))
        return w;

    return w / m_total;
}

void SplitterNode::setWeight(Port& port, Frac weight)
{
    if (m_weightMap.contains(&port)) {
        m_total -= m_weightMap[&port];
        m_weightMap[&port] = weight;
        m_total += weight;
    }
}

// FIX: implement proper portRate for splitter (proportional weight-based flow)
Frac SplitterNode::portRate(const Port* port) const
{
    return Frac(0);
}

QString SplitterNode::getHeaderInfo() const
{
    QStringList proportions;
    for (auto& p : m_outputs) {
        Frac w = m_weightMap.value(p.get(), Frac(0));
        Frac prop = m_total > 0 ? w / m_total : Frac(0);
        proportions << QString("%1/%2").arg(prop.numerator()).arg(prop.denominator());
    }
    return QString("┌─ [%1] Splitter  \"%2\"  (%3)")
               .arg(index())
               .arg(m_name)
               .arg(proportions.join("  "));
}

QJsonObject SplitterNode::getJsonNode() const
{
    QJsonObject obj = AbstractNode::getJsonNode();
    QJsonArray weights;
    for (auto& p : outputs()) {
        Frac w = m_weightMap.value(p.get());
        weights.append(QJsonObject{{"n", w.numerator()}, {"d", w.denominator()}});
    }

    obj["weights"] = weights;
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
