#include "ExtractionNode.h"
#include "core/types/Types.h"

ExtractionNode::ExtractionNode(
    Factory& parentFactory,
    const ExtractionRecipe& recipe,
    int tier,
    QString name,
    QUuid id)
    : MachineNode(parentFactory, name, 1, -1, id)
    , m_tier(tier)
{
    m_type = NodeType::Extraction;
    m_machineLimit = 1;
    setRecipe(&recipe);
}

ExtractionNode::~ExtractionNode()
{
    deletePorts();
}

void ExtractionNode::buildPortsFromRecipe()
{
    if (!recipe())
        return;
    m_outputs.push_back(std::make_unique<Port>(*this, recipe()->resource, PortType::Output));
}

const Machine* ExtractionNode::extractor() const
{
    const ExtractionRecipe* r = recipe();
    if(! r || !r->family || r->family->tiers.isEmpty())
        return nullptr;
    return m_machine;
}

QString ExtractionNode::getHeaderInfo() const
{
    static const QMap<NodePurity, QString> purityNames = {
        { NodePurity::Impure, "Impure" },
        { NodePurity::Normal, "Normal" },
        { NodePurity::Pure, "Pure" },
    };
    QString limitStr = m_machineLimit >= 0
        ? QString("  [limit: %1]").arg(boost::rational_cast<double>(m_machineLimit), 0, 'f', 2)
        : QString();
    return QString("┌─ [%1] %2  \"%3\"  x%4%5\n│  resource : %6  purity: %7")
               .arg(index())
               .arg(machineName())
               .arg(m_name)
               .arg(boost::rational_cast<double>(m_machineCount), 0, 'f', 2)
               .arg(limitStr)
               .arg(recipe() ? recipe()->resource->itemName : "none")
               .arg(purityNames.value(m_purity));
}

QJsonObject ExtractionNode::getJsonNode() const
{
    QJsonObject obj = MachineNode::getJsonNode();
    obj["tier"] = m_tier;
    obj["purity"] = static_cast<int>(m_purity);
    return obj;
}

Frac ExtractionNode::basePortRate(const Port* port) const
{
    const auto* machine = dynamic_cast<const ExtractionMachine*>(extractor());
    if (!machine)
        return Frac(0);
    Frac base = Frac (machine->itemsPerCycle * 4, (int)(machine->extractCycleTime * 4)) * Frac(60);
    const ExtractionRecipe* r = recipe();
    if (r && r->resource && (r->resource->form == Form::Liquid || r->resource->form == Form::Gas))
        base /= 1000;

    base *= m_overclock;

    switch (m_purity) {
    case NodePurity::Impure:
        return base * Frac(1, 2);
    case NodePurity::Pure:
        return base * Frac(2, 1);
    default:
        return base;
    }
}

// QString ExtractionNode::getExtractorName() const
// {
//     return extractor() ? extractor()->machineName : "No Machine";
// }

const ExtractionRecipe* ExtractionNode::recipe() const
{
    return static_cast<const ExtractionRecipe*>(m_recipe);
}

void ExtractionNode::setTierInt(int t)
{
    const ExtractionRecipe* r = recipe();
    if (!r || !r->family || t < 0 || t >= r->family->tiers.size())
        return;
    m_tier = t;
    m_machine = r->family->tiers[m_tier];
    // emit tierChanged();
    // emit recipeChanged();
}

void ExtractionNode::setRecipe(const ExtractionRecipe* recipe)
{
    if (m_recipe == recipe)
        return;

    m_recipe = recipe;
    // if (recipe->family->tiers.size() < m_tier)
    m_machine = static_cast<ExtractionMachine*>(recipe->family->tiers[m_tier]);
    deletePorts();
    buildPortsFromRecipe();
}

