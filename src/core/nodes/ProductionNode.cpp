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

    for (auto in : recipe()->inputs)
        m_inputs.push_back(std::make_unique<Port>(*this, in.first, PortType::Input));

    for (auto out : recipe()->outputs)
        m_outputs.push_back(std::make_unique<Port>(*this, out.first, PortType::Output));
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
    const auto& list = (port->type == PortType::Input) ? recipe()->inputs : recipe()->outputs;
    auto it = std::find_if(list.begin(), list.end(), [&](const auto& p) { return p.first == port->item; });
    Frac perCycle = (it != list.end()) ? it->second : Frac(0);
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
