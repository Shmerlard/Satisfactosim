#pragma once

#include "MachineNode.h"
#include "core/types/Types.h"

class ExtractionNode : public MachineNode {
    Q_OBJECT;
    Q_PROPERTY(int purity READ purityInt WRITE setPurityInt NOTIFY purityChanged)
    Q_PROPERTY(int tier READ tier WRITE setTierInt NOTIFY tierChanged)
    Q_PROPERTY(int tierCount READ tierCount CONSTANT)

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
    int purityInt() const { return static_cast<int>(m_purity); }
    void setPurity(NodePurity p) { m_purity = p; emit purityChanged(); }
    void setPurityInt(int p) { setPurity(static_cast<NodePurity>(p)); }
    int tier() const { return m_tier; }
    int tierCount() const { return recipe() && recipe()->family ? recipe()->family->tiers.size() : 1; }
    void setTierInt(int t);

    void setRecipe(const ExtractionRecipe* recipe);
    const ExtractionRecipe* recipe() const override;
    Frac basePortRate(const Port* port) const override;
    QJsonObject getJsonNode() const override;

signals:
    void purityChanged();
    void tierChanged();
};
