#pragma once

#include <QList>
#include <QMap>
#include <QObject>
#include <QString>
#include "boost/rational.hpp"

struct Machine;
struct Recipe;
struct ExtractionRecipe;
struct ProductionRecipe;
struct MachineFamily;

using Frac = boost::rational<int32_t>;

enum class NodePurity { Impure, Normal, Pure };
enum class Form { Solid, Liquid, Gas };

inline NodePurity nodePurityFromString(const QString& s, NodePurity fallback = NodePurity::Normal)
{
    if (s == "impure") return NodePurity::Impure;
    if (s == "pure")   return NodePurity::Pure;
    if (s == "normal") return NodePurity::Normal;
    return fallback;
}

// ------------------- ITEMS    --------------------
struct Item {
    QString itemClass;
    QString itemName;
    QString iconPath; // FIX: may not be needed
    float sinkPoints;
    float energy;
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
    float basePowerConsumption;
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

    QMap<Item*, Frac> outputs;
    virtual ~Recipe() = 0;
};
inline Recipe::~Recipe() = default;

struct ProductionRecipe : Recipe {
    bool isAlternate = false;
    Frac recipeTime;
    Machine* producedIn;
    QMap<Item*, Frac> inputs;
};

struct ExtractionRecipe : Recipe {
    MachineFamily* family;
    Resource* resource;
};
