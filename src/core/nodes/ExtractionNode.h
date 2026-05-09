#pragma once

#include "MachineNode.h"
#include "core/types/Types.h"

class ExtractionNode : public MachineNode {
    Q_OBJECT;

    friend class SessionManager;
    friend class Factory;

private:
    explicit ExtractionNode(
        Factory& parentFactory,
        const ExtractionRecipe& recipe,
        int tier = 0,
        QString name = QString(),
        QUuid id = QUuid());

    int m_tier = 0;
    NodePurity m_purity = NodePurity::Normal;

    void buildPortsFromRecipe() override;

public:
    ~ExtractionNode() override;
    const Machine* extractor() const;
    NodePurity purity() const { return m_purity; }
    void setPurity(NodePurity purity) { m_purity = purity; }
    int tier() const { return m_tier; }

    void setRecipe(const ExtractionRecipe* recipe);
    const ExtractionRecipe* recipe() const override;
    float portRate(const Port* port) const override;
    QJsonObject getJsonNode() const override;
    QString getExtractorName() const; // FIX: remove
    QString machineName() const override;
    QString getMachineIcon() const;
};
