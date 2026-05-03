#pragma once
#include "MachineNode.h"
#include "src/core/types/Types.h"


class ProductionNode : public MachineNode {
    friend class SessionManager;

private:
    explicit ProductionNode(
            Factory& parentFactory,
            const Recipe& recipe,
            QString name = QString());


    void clearPorts() override;
    void buildPortsFromRecipe() override;

public:
    ~ProductionNode() override;

    void setRecipe(const ProductionRecipe* recipe);
    const ProductionRecipe* recipe() const override;
    Machine* machine() const;

    float portRate(const Port* port) const override;
    QJsonObject getJsonNode() const override;
    QString getMachineName() const;                     // FIX: redundent
    QString machineName() const override;
    QString getMachineIcon() const;
    int hierarchyLevel() const override { return 3; }
};
