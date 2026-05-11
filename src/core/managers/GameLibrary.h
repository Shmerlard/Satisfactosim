#pragma once

#include "core/nodes/ProductionNode.h"
#include "core/types/Types.h"
#include <QMap>
#include <QString>
#include <stdlib.h>

class GameLibrary {
private:
    GameLibrary() = default;
    void parseItems();
    void parseMachines();
    void parseRecipes();
    void parseExtractionRecipes();

    QMap<QString, Item*> m_items; // (itemClass, Item*)
    QMap<QString, Machine*> m_machines;
    QMap<QString, ProductionMachine*> m_productionMachines;

    // TODO: unique pointers?
    QMap<QString, Recipe*> m_recipes;
    QMap<QString, ExtractionRecipe*> m_extractionRecipes;
    QMap<QString, Resource*> m_resources;
    QMap<QString, ExtractionMachine*> m_extractors;
    QMap<QString, MachineFamily*> m_machineFamilies;

public:
    static GameLibrary& get()
    {
        static GameLibrary inst;
        return inst;
    }
    ~GameLibrary();

    // FIX: is it needed
    GameLibrary(const GameLibrary&) = delete;
    void operator=(const GameLibrary&) = delete;

    void loadData();
    const Recipe* getRecipeByClass(const QString& name) const;
    const ExtractionRecipe* getExtRecipeByResource(const Resource* resource) const;
    const ExtractionRecipe* getExtRecipeByResource(const QString& name) const;
    const ExtractionRecipe* getExtRecipeByClass(const QString& rClass) const;
    const Machine* getMachine(const QString& name) const;

    const QMap<QString, Item*>& Items() const;
    const QMap<QString, Machine*>& Machines() const;
    const QMap<QString, Recipe*>& Recipes() const;

    const Item* getItem(const QString& name) const;
};
