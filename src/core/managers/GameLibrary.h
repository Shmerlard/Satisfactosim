#pragma once

#include "core/nodes/ProductionNode.h"
#include "core/types/Types.h"
#include <QMap>
#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <stdlib.h>

class GameLibrary : public QObject {
    // Q_OBJECT;
    // QML_ELEMENT;
    // QML_SINGLETON;
    // Q_PROPERTY(QVariantList itemsList READ itemsList CONSTANT);
    // Q_PROPERTY(QVariantList machinesList READ machinesList CONSTANT);
    // Q_PROPERTY(QVariantList recipesList READ recipesList CONSTANT);

private:
    GameLibrary() = default;
    void parseItems();
    void parseMachines();
    void parseRecipes();
    void parseExtractionRecipes();
    void connectRecipes();

    QMap<QString, Item*> m_items; // (itemClass, Item*)
    QMap<QString, Machine*> m_machines;
    QMap<QString, Recipe*> m_recipes;
    QMap<QString, ExtractionRecipe*> m_extractionRecipes;
    QMap<QString, Item*> m_resources;
    QMap<QString, Machine*> m_extractors;
    QMap<QString, MachineFamily*> m_machineFamilies;

public:
    // static GameLibrary* create(QQmlEngine*, QJSEngine*)
    // {
    //     QQmlEngine::setObjectOwnership(&get(), QQmlEngine::CppOwnership);
    //     return &get();
    // }
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
    // const Recipe* getRecipeByName(const QString& name) const;
    const ExtractionRecipe* getExtRecipeByResource(const Item* resource);
    const ExtractionRecipe* getExtRecipeByResource(const QString name);
    const Machine* getMachine(const QString& name) const;

    const QMap<QString, Item*> Items() const;
    const QMap<QString, Machine*> Machines() const;
    const QMap<QString, Recipe*> Recipes() const;

    // QVariantList itemsList() const;
    // QVariantList machinesList() const;
    // QVariantList recipesList() const;

// public slots:
    const Item* getItem(const QString& name) const;
};
