#pragma once

#include "MachineNode.h"
#include "core/types/Types.h"
struct ExtractionRecipe;
struct Machine;

class ExtractionNode : public  MachineNode {
    friend class SessionManager;

private:
    explicit ExtractionNode(Factory& parentFactory, const ExtractionRecipe& recipe , int tier = 0, QString name = QString());
    const ExtractionRecipe* m_recipe;
    int m_tier;
    NodePurity m_purity = NodePurity::Normal;

    void clearPorts() override;
    void buildPortsFromRecipe() override;

public:
    ~ExtractionNode() override;
    const ExtractionRecipe* currentRecipe() const { return m_recipe; }
    const Machine* extractor() const;
    NodePurity purity() const { return m_purity; }
    void setPurity(NodePurity purity) { m_purity = purity; }
    int tier() const { return m_tier; }

    float portRate(const Port* port) const override;
    QJsonObject getJsonNode() const override;
    QString getExtractorName() const; // FIX: remove
    QString machineName() const override;
    QString getMachineIcon() const;
    int hierarchyLevel() const override { return 2; }
};
