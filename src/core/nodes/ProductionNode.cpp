#include "ProductionNode.h"
#include "Factory.h"
#include "Port.h"
#include "core/types/Types.h"

ProductionNode::ProductionNode(
    Factory& parentFactory,
    // TODO: header declares const ProductionRecipe& but this takes const Recipe& — mismatch with header
    const Recipe& recipe,
    QString name)
    : MachineNode(parentFactory, name)
{
    m_recipe = &recipe;
    m_parentFactory->addNode(*this);
    buildPortsFromRecipe();
}

ProductionNode::~ProductionNode()
{
    clearPorts();
}

void ProductionNode::setRecipe(const ProductionRecipe* recipe)
{
    if (m_recipe == recipe)
        return;

    m_recipe = recipe;
    clearPorts();
    buildPortsFromRecipe();
    // TODO: call update, keep inputs the same if
    // the inputs ingredients are the same
}

QJsonObject ProductionNode::getJsonNode() const
{
    QJsonObject obj = MachineNode::getJsonNode();
    obj["type"] = "production";
    obj["recipe"] = recipe() ? recipe()->recipeClass : "";
    return obj;
}

float ProductionNode::portRate(const Port* port) const
{
    if (!recipe() || recipe()->recipeTime <= 0.0f)
        return 0.0f;
    float perCycle = port->type == PortType::Input
        ? recipe()->inputs.value(port->item, 0.0f)
        : recipe()->outputs.value(port->item, 0.0f);
    return perCycle / recipe()->recipeTime * 60.0f;
}

Machine* ProductionNode::machine() const
{
    // TODO: no null check on recipe() — crashes if m_recipe is null
    return recipe()->producedIn;
}

void ProductionNode::clearPorts()
{
    auto disconnectAll = [](const QList<Port*>& ports) {
        for (Port* port : ports) {
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
    if (!recipe())
        return;

    auto inIt = recipe()->inputs.constBegin();
    while (inIt != recipe()->inputs.constEnd()) {
        m_inputs.append(new Port(*this, inIt.key(), PortType::Input));
        ++inIt;
    }
    auto outIt = recipe()->outputs.constBegin();
    while (outIt != recipe()->outputs.constEnd()) {
        m_outputs.append(new Port(*this, outIt.key(), PortType::Output));
        ++outIt;
    }
}

// TODO: getMachineName and machineName are identical — one is redundant, remove getMachineName
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

const ProductionRecipe* ProductionNode::recipe() const 
{
    return static_cast<const ProductionRecipe*>(m_recipe);
}
