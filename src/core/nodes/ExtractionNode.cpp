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

QJsonObject ExtractionNode::getJsonNode() const
{
    QJsonObject obj = MachineNode::getJsonNode();
    obj["tier"] = m_tier;
    obj["purity"] = static_cast<int>(m_purity);
    return obj;
}

float ExtractionNode::portRate(const Port* port) const
{
    const auto* machine = dynamic_cast<const ExtractionMachine*>(extractor());
    if (!machine)
        return 0.0f;
    float base = (float)machine->itemsPerCycle / machine->extractCycleTime * 60.0f;
    const ExtractionRecipe* r = recipe();
    if (r && r->resource && (r->resource->form == Form::Liquid || r->resource->form == Form::Gas))
        base /= 1000.0f;
    switch (m_purity) {
    case NodePurity::Impure:
        return base * 0.5f;
    case NodePurity::Pure:
        return base * 2.0f;
    default:
        return base;
    }
}

QString ExtractionNode::getExtractorName() const
{
    return extractor() ? extractor()->machineName : "No Machine";
}

const ExtractionRecipe* ExtractionNode::recipe() const
{
    return static_cast<const ExtractionRecipe*>(m_recipe);
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

