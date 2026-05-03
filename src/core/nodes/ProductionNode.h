#pragma once
#include "MachineNode.h"

struct Recipe;
struct Machine;

class ProductionNode : public MachineNode {

    friend class SessionManager;

private:
    explicit ProductionNode(Factory& parentFactory, const Recipe& recipe, QString name = QString());

    const Recipe* m_currentRecipe;

    void clearPorts() override;
    void buildPortsFromRecipe() override;

public:
    ~ProductionNode() override;

    const Recipe* currentRecipe() const { return m_currentRecipe; }
    void setRecipe(const Recipe* recipe);
    Machine* machine() const;

    float portRate(const Port* port) const override;
    QJsonObject getJsonNode() const override;
    QString getMachineName() const; // FIX: redundent
    QString machineName() const override;
    QString getMachineIcon() const;
    int hierarchyLevel() const override { return 3; }
};
