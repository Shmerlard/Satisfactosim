#include "ProductionNode.h"
#include "Factory.h"
#include "Port.h"
#include "core/types/Types.h"

ProductionNode::ProductionNode(
    Factory& parentFactory,
    const ProductionRecipe& recipe,
    QString name,
    QUuid id)
    : MachineNode(parentFactory, name, 1, -1, id)
{
    m_type = NodeType::Production;
    setRecipe(&recipe);
}

void ProductionNode::buildPortsFromRecipe()
{
    if (!recipe())
        return;

    auto inIt = recipe()->inputs.constBegin();
    while (inIt != recipe()->inputs.constEnd()) {
        m_inputs.push_back(std::make_unique<Port>(*this, inIt.key(), PortType::Input));
        ++inIt;
    }
    auto outIt = recipe()->outputs.constBegin();
    while (outIt != recipe()->outputs.constEnd()) {
        m_outputs.push_back(std::make_unique<Port>(*this, outIt.key(), PortType::Output));
        ++outIt;
    }
}

ProductionNode::~ProductionNode()
{
    deletePorts();
}

void ProductionNode::setRecipe(const ProductionRecipe* recipe)
{
    if (m_recipe == recipe)
        return;

    m_recipe = recipe;
    m_machine = static_cast<const ProductionRecipe*>(m_recipe)->producedIn;
    deletePorts();
    buildPortsFromRecipe();
}

QJsonObject ProductionNode::getJsonNode() const
{
    QJsonObject obj = MachineNode::getJsonNode();
    return obj;
}

Frac ProductionNode::portRate(const Port* port) const
{
    if (!recipe() || recipe()->recipeTime <= 0)
        return Frac(0);
    Frac perCycle = (port->type == PortType::Input)
        ? recipe()->inputs.value(port->item, Frac(0))
        : recipe()->outputs.value(port->item, Frac(0));
    return perCycle / recipe()->recipeTime * 60;
    // FIX: might be better to just move it to Port and read from there
}

Machine* ProductionNode::machine() const
{
    if (!recipe())
        return nullptr;
    return recipe()->producedIn;
}

const ProductionRecipe* ProductionNode::recipe() const
{
    return static_cast<const ProductionRecipe*>(m_recipe);
}
