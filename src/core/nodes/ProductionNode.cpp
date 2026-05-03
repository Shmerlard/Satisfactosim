#include "ProductionNode.h"
#include "Factory.h"
#include "Port.h"
#include "core/types/Types.h"

ProductionNode::ProductionNode(
    Factory& parentFactory,
    const Recipe& recipe,
    QString name)
    : MachineNode(parentFactory, name)
    , m_currentRecipe(&recipe)
{
    m_parentFactory->addNode(*this);
    buildPortsFromRecipe();
}


ProductionNode::~ProductionNode()
{
    clearPorts();
}

void ProductionNode::setRecipe(const Recipe* recipe)
{
    if (m_currentRecipe == recipe)
        return;

    m_currentRecipe = recipe;
    clearPorts();
    buildPortsFromRecipe();
    // TODO: call update, keep inputs the same if
    // the inputs ingredients are the same
}


QJsonObject ProductionNode::getJsonNode() const
{
    QJsonObject obj = MachineNode::getJsonNode();
    obj["type"]   = "production";
    obj["recipe"] = m_currentRecipe ? m_currentRecipe->recipeClass : "";
    return obj;
}

float ProductionNode::portRate(const Port* port) const
{
    if (!m_currentRecipe || m_currentRecipe->recipeTime <= 0.0f)
        return 0.0f;
    float perCycle = port->type == PortType::Input
        ? m_currentRecipe->inputs.value(port->item, 0.0f)
        : m_currentRecipe->outputs.value(port->item, 0.0f);
    return perCycle / m_currentRecipe->recipeTime * 60.0f;
}

Machine* ProductionNode::machine() const
{
    return m_currentRecipe->producedIn;
}

void ProductionNode::clearPorts()
{
    auto disconnectAll = [](const QList<Port*>& ports) {
        for (Port* port: ports) {
            for (Port* peer : port->connectedTo) {
                peer->connectedTo.removeAll(port);
            }
        }
    };
    disconnectAll(m_inputs);
    disconnectAll(m_outputs);
    qDeleteAll(m_inputs);
    qDeleteAll(m_outputs);
    m_inputs.clear();
    m_outputs.clear();
}

void ProductionNode::buildPortsFromRecipe()
{
    if (!m_currentRecipe)
        return;

    auto inIt = m_currentRecipe->inputs.constBegin();
    while (inIt != m_currentRecipe->inputs.constEnd()) {
        m_inputs.append(new Port(*this, inIt.key(), PortType::Input));
        ++inIt;
    }
    auto outIt = m_currentRecipe->outputs.constBegin();
    while (outIt != m_currentRecipe->outputs.constEnd()) {
        m_outputs.append(new Port(*this, outIt.key(), PortType::Output));
        ++outIt;
    }
}

QString ProductionNode::getMachineName() const
{
    return machine() ? machine()->machineName : "No Machine";
}

QString ProductionNode::machineName() const 
{
    return machine() ? machine()->machineName : "No Machine";
}

QString ProductionNode::getMachineIcon() const
{
    return machine() ? machine()->iconPath : "";
}
