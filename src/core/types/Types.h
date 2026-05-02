#pragma once
// #include "core/nodes/ProductionNode.h"

enum class NodePurity { Impure, Normal, Pure };
#include <QList>
#include <QMap>
#include <QObject>
#include <QString>

struct Machine;
struct Recipe;
struct ExtractionRecipe;

struct Item {
    QString itemClass;
    QString itemName;
    float sinkPoints;
    QString form;
    QList<Recipe*> producedBy;
    QList<Recipe*> usedIn;
    QString iconPath; // FIX: may not be needed
    bool isResource;
    QList<ExtractionRecipe*> extractedBy;
};

struct ExtractorSettings {
    QString extractorType; // ["Miner", "Waterpipe"...]
    int tier;               // only applies to miner
    float extractCycleTime;
    int itemsPerCycle;
    QList<Item*> allowedResources;
    QList<ExtractionRecipe*> extractionRecipes;
};

struct Machine {
    QString machineClass;
    QString machineName;
    float powerConsumption;
    QList<Recipe*> recipes;
    QString iconPath;
    bool isExtractor;
    ExtractorSettings* extractorSettings = nullptr;
};

struct Recipe {
    QString recipeClass;
    QString recipeName;
    bool isAlternate = false;
    Machine* producedIn;
    float recipeTime;

    QMap<Item*, float> inputs;
    QMap<Item*, float> outputs;
};

struct MachineFamily {
    QString familyName;
    QList<Machine*> tiers;
};

struct ExtractionRecipe {
    QString recipeClass;
    QString recipeName;
    MachineFamily* family;
    Item* resource;
};
