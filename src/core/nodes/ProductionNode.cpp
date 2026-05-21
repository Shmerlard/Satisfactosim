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

QString ProductionNode::getHeaderInfo() const
{
    QString recipeName = "none", machineName, cycleInfo;
    if (const ProductionRecipe* r = recipe()) {
        recipeName = r->recipeClass;
        if (r->producedIn)
            machineName = r->producedIn->machineName;
        cycleInfo = QString("  (%1s, %2MW)")
                        .arg(boost::rational_cast<double>(r->recipeTime), 0, 'f', 1)
                        .arg(r->producedIn ? r->producedIn->basePowerConsumption : 0.f, 0, 'f', 0);
    }
    QString limitStr = m_machineLimit >= 0
        ? QString("  [limit: %1]").arg(boost::rational_cast<double>(m_machineLimit), 0, 'f', 2)
        : QString();
    return QString("┌─ [%1] %2  \"%3\"  x%4%5\n│  recipe : %6%7")
               .arg(index())
               .arg(machineName)
               .arg(m_name)
               .arg(boost::rational_cast<double>(m_machineCount), 0, 'f', 2)
               .arg(limitStr)
               .arg(recipeName)
               .arg(cycleInfo);
}

QJsonObject ProductionNode::getJsonNode() const
{
    QJsonObject obj = MachineNode::getJsonNode();
    return obj;
}

Frac ProductionNode::basePortRate(const Port* port) const
{
    if (!recipe() || recipe()->recipeTime <= 0)
        return Frac(0);
    const auto& list = (port->type == PortType::Input) ? recipe()->inputs : recipe()->outputs;
    auto it = std::find_if(list.begin(), list.end(), [&](const auto& p) { return p.first == port->item; });

    Frac perCycle = (it != list.end()) ? it->second : Frac(0);
    Frac basePerMinute = perCycle / recipe()->recipeTime * 60;
    basePerMinute *= somersloopFactor();
    basePerMinute *= m_overclock;

    return basePerMinute;
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
