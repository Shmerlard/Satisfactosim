#pragma once

#include <QList>
#include <QMap>
#include <QObject>
#include <QString>

struct Machine;
struct Recipe;
struct ExtractionRecipe;
struct ProductionRecipe;
struct MachineFamily;

enum class NodePurity { Impure, Normal, Pure };
enum class Form { Solid, Liquid, Gas };

// ------------------- ITEMS    --------------------
struct Item {
    QString itemClass;
    QString itemName;
    QString iconPath; // FIX: may not be needed
    float sinkPoints;
    float energy;
    // QString form;
    Form form;
    QList<Recipe*> producedBy;
    QList<Recipe*> usedIn;
    virtual ~Item() = 0;
};
inline Item::~Item() = default;

struct Resource : Item {
    // QList<ExtractionRecipe*> extractedBy;
};

struct Component : Item {
};

// struct Equipment : Item {
//
// };

// ------------------- MACHINES --------------------
struct Machine {
    QString machineClass;
    QString machineName;
    float basePowerConsumption;                 // TODO: may be a problem when talking about variable power consumption
    QString iconPath;

    virtual ~Machine() = 0;
};
inline Machine::~Machine() = default;

struct ProductionMachine : Machine {
    QList<ProductionRecipe*> recipes;
};

struct ExtractionMachine : Machine {
    MachineFamily* type;
    int tier;
    float extractCycleTime;
    int itemsPerCycle;
};

struct PowerGenMachine : Machine {
    // FIX: list of accepted fuels
    // power generation
};

struct MachineFamily {
    QString familyName;
    QList<ExtractionMachine*> tiers;
    QList<Resource*> allowedResources;
    QList<ExtractionRecipe*> extractionRecipes;
};

// ------------------- RECIPES --------------------
struct Recipe {
    QString recipeClass;
    QString recipeName;

    QMap<Item*, float> outputs;
    virtual ~Recipe() = 0;
};
inline Recipe::~Recipe() = default; // TODO: make sure it works

struct ProductionRecipe : Recipe {
    bool isAlternate = false;
    float recipeTime;
    Machine* producedIn;
    QMap<Item*, float> inputs;
};

struct ExtractionRecipe : Recipe {
    MachineFamily* family;
    Resource* resource;
};
