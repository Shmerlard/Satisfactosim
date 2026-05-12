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
    // m_recipe = &recipe;

    // buildPortsFromRecipe();
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
    // if (!recipe() || !recipe()->family || recipe()->family->tiers.isEmpty())
    //     return nullptr;
    // int tier = qBound(0, m_tier, recipe()->family->tiers.size() - 1);
    // return recipe()->family->tiers[tier];
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
    // const Machine* machine = extractor();
    // if (!machine || !machine->extractorSettings)
    //     return 0.0f;
    // const auto* s = machine->extractorSettings;
    // float base = s->itemsPerCycle / s->extractCycleTime * 60.0f;
    // if (m_recipe && m_recipe->resource && (m_recipe->resource->form == "RF_LIQUID" || m_recipe->resource->form == "RF_GAS"))
    //     base /= 1000.0f;
    // switch (m_purity) {
    // case NodePurity::Impure:
    //     return base * 0.5f;
    // case NodePurity::Pure:
    //     return base * 2.0f;
    // default:
    //     return base;
    // }
    return 0.f;
}

QString ExtractionNode::getExtractorName() const
{
    return extractor() ? extractor()->machineName : "No Machine";
}

// QString ExtractionNode::machineName() const
// {
//     return extractor() ? extractor()->machineName : "No Machine";
// }
//
// QString ExtractionNode::machineIcon() const
// {
//     const Machine* m = extractor();
//     return m ? "image://assets/machine/" + m->machineClass : QString();
// }

const ExtractionRecipe* ExtractionNode::recipe() const
{
    return static_cast<const ExtractionRecipe*>(m_recipe);
}

void ExtractionNode::setRecipe(const ExtractionRecipe* recipe)
{
    if (m_recipe == recipe)
        return;

    m_recipe = recipe;
    m_machine = static_cast<ExtractionMachine*>(recipe->family->tiers[m_tier]);
    deletePorts();
    buildPortsFromRecipe();
}

