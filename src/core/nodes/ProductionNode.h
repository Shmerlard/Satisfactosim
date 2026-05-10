#pragma once
#include "MachineNode.h"
#include "src/core/types/Types.h"


class ProductionNode : public MachineNode {
    Q_OBJECT;

    friend class SessionManager;
    friend class Factory;
private:
    explicit ProductionNode(
            Factory& parentFactory,
            const ProductionRecipe& recipe,
            QString name = QString(),
            QUuid id = QUuid());


    void buildPortsFromRecipe() override;

public:
    ~ProductionNode() override;

    void setRecipe(const ProductionRecipe* recipe);
    const ProductionRecipe* recipe() const override;
    Machine* machine() const;

    float portRate(const Port* port) const override;
    QJsonObject getJsonNode() const override;
    QString machineName() const override;
    QString getMachineIcon() const;
};
