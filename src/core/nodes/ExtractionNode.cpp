#include "ExtractionNode.h"
#include "core/types/Types.h"

ExtractionNode::~ExtractionNode() = default;

ExtractionNode::ExtractionNode(
    Factory& parentFactory,
    const ExtractionRecipe& recipe,
    int tier,
    QString name)
    : MachineNode(parentFactory, name)
    , m_tier(tier)
{
    m_recipe = &recipe;
    m_parentFactory->addNode(*this);

    buildPortsFromRecipe();
}
void ExtractionNode::buildPortsFromRecipe()
{
    if (!m_recipe)
        return;

    Port* out = new Port(*this, recipe()->resource, PortType::Output);
    m_outputs.append(out);
}

void ExtractionNode::clearPorts()
{
    for (Port* port : m_outputs) {
        for (Port* peer : port->connectedTo) {
            peer->connectedTo.removeAll(port);
        }
    }
    qDeleteAll(m_outputs);
    m_outputs.clear();
}

const Machine* ExtractionNode::extractor() const
{
    // TODO: mixes m_recipe and recipe() checks inconsistently — use recipe() throughout
    if (!m_recipe || !recipe()->family || recipe()->family->tiers.isEmpty())
        return nullptr;
    int tier = qBound(0, m_tier, recipe()->family->tiers.size() - 1);
    return recipe()->family->tiers[tier];
}

QJsonObject ExtractionNode::getJsonNode() const
{
    static const QMap<NodePurity, QString> purityNames = {
        { NodePurity::Impure, "impure" },
        { NodePurity::Normal, "normal" },
        { NodePurity::Pure, "pure" },
    };
    QJsonObject obj = MachineNode::getJsonNode();
    obj["type"] = "extraction";
    // TODO: no null check on recipe()->resource — crashes if resource is null
    obj["resource"] = recipe() ? recipe()->resource->itemClass : "";
    obj["tier"] = m_tier;
    obj["purity"] = purityNames.value(m_purity, "normal");
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

QString ExtractionNode::machineName() const
{
    return extractor() ? extractor()->machineName : "No Machine";
}

QString ExtractionNode::getMachineIcon() const
{
    return extractor() ? extractor()->iconPath : "";
}

const ExtractionRecipe* ExtractionNode::recipe() const 
{
    return static_cast<const ExtractionRecipe*>(m_recipe);
}
