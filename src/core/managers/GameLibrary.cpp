#include "GameLibrary.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

namespace {
Form formFromString(const QString& str)
{
    if (str == "RF_LIQUID")
        return Form::Liquid;
    if (str == "RF_GAS")
        return Form::Gas;
    return Form::Solid;
}

Component* componentFromJsonObject(const QJsonValue& value)
{

    QJsonObject obj = value.toObject();
    Component* i = new Component;

    i->itemClass = obj["Class"].toString();
    i->itemName = obj["DisplayName"].toString();
    i->sinkPoints = obj["SinkPoints"].toString().toDouble();
    i->form = formFromString(obj["Form"].toString());
    i->iconPath = QString("assets/icons/items/%1.png").arg(i->itemClass);
    // TODO: energy

    return i;
}

// FIX: its the same as above, Maybe add a template
Resource* resourceFromJsonObject(const QJsonValue& value)
{

    QJsonObject obj = value.toObject();
    Resource* i = new Resource;

    i->itemClass = obj["Class"].toString();
    i->itemName = obj["DisplayName"].toString();
    i->sinkPoints = obj["SinkPoints"].toString().toDouble();
    i->form = formFromString(obj["Form"].toString());
    i->iconPath = QString("assets/icons/items/%1.png").arg(i->itemClass);
    // TODO: energy

    return i;
}

ProductionMachine* productionMachineFromJsonObject(const QJsonValue& value)
{
    QJsonObject obj = value.toObject();
    ProductionMachine* m = new ProductionMachine();

    m->machineClass = obj["Class"].toString();
    m->machineName = obj["DisplayName"].toString();
    m->basePowerConsumption = obj["PowerConsumption"].toString().toDouble();
    m->iconPath = QString("assets/icons/machines/%1.png").arg(m->machineName);

    return m;
}

ExtractionMachine* extractionMachineFromJsonObject(const QJsonValue& value, QMap<QString, MachineFamily*>& families)
{
    QJsonObject obj = value.toObject();

    ExtractionMachine* m = new ExtractionMachine();

    m->machineClass = obj["Class"].toString();
    m->machineName = obj["DisplayName"].toString();
    m->basePowerConsumption = obj["PowerConsumption"].toString().toDouble();
    m->iconPath = QString("assets/icons/machines/%1.png").arg(m->machineName);

    QString extrTypeName = obj["ExtractorTypeName"].toString();
    if (!families.contains(extrTypeName)) {
        MachineFamily* newFamily = new MachineFamily();
        newFamily->familyName = extrTypeName;

        families.insert(extrTypeName, newFamily);
    }
    MachineFamily* fam = families.value(extrTypeName);
    fam->tiers.append(m);

    m->type = fam;
    m->tier = fam->tiers.indexOf(m); // FIX: problematic since we assume that they are inserted by order
    m->extractCycleTime = obj["ExtractCycleTime"].toDouble();
    m->itemsPerCycle = obj["ItemsPerCycle"].toInt();
    return m;
}

} // namespace

GameLibrary::~GameLibrary()
{
    qDeleteAll(m_items);
    qDeleteAll(m_machines);
    qDeleteAll(m_recipes);
    qDeleteAll(m_machineFamilies);
}

void GameLibrary::loadData()
{
    parseItems();
    parseMachines();
    parseRecipes();

    parseExtractionRecipes();

    std::cout << "Data Loaded Succefully" << '\n';
    std::cout << "machines: " << m_machines.size() << '\n';
    std::cout << "items: " << m_items.size() << '\n';
    std::cout << "recipes: " << m_recipes.size() << '\n';
}

void GameLibrary::parseItems()
{
    QFile itemsFile(":/assets/jsons/items.json");

    if (!itemsFile.open(QIODevice::ReadOnly))
        return;
    QJsonDocument doc = QJsonDocument::fromJson(itemsFile.readAll());
    QJsonObject root = doc.object();

    for (const QJsonValue& value : root["Components"].toArray()) {
        Item* i = componentFromJsonObject(value);
        m_items.insert(i->itemClass, i);
    }
    for (const QJsonValue& value : root["Resources"].toArray()) {
        auto* i = resourceFromJsonObject(value);
        m_items.insert(i->itemClass, i);
        m_resources.insert(i->itemClass, i);
    }
}

void GameLibrary::parseMachines()
{
    QFile machinesFile(":/assets/jsons/machines.json");

    if (!machinesFile.open(QIODevice::ReadOnly))
        return;
    QJsonDocument doc = QJsonDocument::fromJson(machinesFile.readAll());
    QJsonObject root = doc.object();

    for (const QJsonValue& value : root["ProductionMachines"].toArray()) {
        ProductionMachine* m = productionMachineFromJsonObject(value);
        m_productionMachines.insert(m->machineClass, m);
        m_machines.insert(m->machineClass, m);
    }

    for (const QJsonValue& value : root["ExtractionMachines"].toArray()) {
        QJsonObject obj = value.toObject();
        ExtractionMachine* m = extractionMachineFromJsonObject(value, m_machineFamilies);

        if (m->type->tiers.size() == 1) {
            if (obj["OnlyAllowCertainResources"].toBool()) {
                QJsonArray allowedResources = obj["AllowedResources"].toArray();
                for (const QJsonValue& res : allowedResources) {
                    QString resClass = res.toString();
                    Resource* resItem = m_resources.value(resClass);
                    if (resItem) {
                        m->type->allowedResources.append(resItem);
                    }
                }
            } else {
                QJsonArray allowedFormsArr = obj["AllowedResourceForms"].toArray();
                for (const QJsonValue& formVal : allowedFormsArr) {
                    Form form = formFromString(formVal.toString());
                    for (Resource* resource : m_resources) {
                        if (resource && resource->form == form) {
                            m->type->allowedResources.append(resource);
                        }
                    }
                }
            }
        }

        m_machines.insert(m->machineClass, m);
        m_extractors.insert(m->machineClass, m);
    }
}

void GameLibrary::parseRecipes()
{
    QFile recipesFile(":/assets/jsons/recipes.json");

    if (!recipesFile.open(QIODevice::ReadOnly))
        return;
    QJsonDocument doc = QJsonDocument::fromJson(recipesFile.readAll());
    QJsonArray rootArray = doc.array();

    for (const QJsonValue& value : rootArray) {
        QJsonObject obj = value.toObject();
        ProductionRecipe* r = new ProductionRecipe();

        r->recipeClass = obj["Class"].toString();
        r->recipeName = obj["DisplayName"].toString().remove("Alternate:");
        r->isAlternate = obj["Alternate"].toBool();
        r->recipeTime = obj["ManufactoringDuration"].toString().toDouble();

        m_recipes.insert(r->recipeClass, r);

        ProductionMachine* machine = m_productionMachines.value(obj["ProducedIn"].toString());
        if (machine) {
            r->producedIn = machine;
            machine->recipes.append(r);
        } else {
            qWarning() << "WARN: no machine found in recipe: " << r->recipeClass;
        }

        QJsonArray ingArr = obj["Ingredients"].toArray();
        for (const QJsonValue& ing : ingArr) {
            QJsonObject ingObj = ing.toObject();
            QString ingClass = ingObj["Item"].toString();
            Item* item = m_items.value(ingClass);
            float ingAmount = ingObj["Amount"].toDouble();

            if (item) {
                if (item->form == Form::Liquid || item->form == Form::Gas)
                    ingAmount /= 1000;
                r->inputs.insert(item, ingAmount);
                item->usedIn.append(r);
            } else {
                qWarning() << "NO SUCH ITEM: " << ingClass;
            }
        }
        QJsonArray prodArr = obj["Product"].toArray();
        for (const QJsonValue& prod : prodArr) {
            QJsonObject prodObj = prod.toObject();
            QString prodClass = prodObj["Item"].toString();
            Item* item = m_items.value(prodClass);
            float prodAmount = prodObj["Amount"].toDouble();
            if (item) {
                if (item->form == Form::Liquid || item->form == Form::Gas)
                    prodAmount /= 1000;

                r->outputs.insert(item, prodAmount);
                item->producedBy.append(r);
            } else {
                qWarning() << "NO SUCH ITEM: " << prodClass;
            }
        }
    }
}

void GameLibrary::parseExtractionRecipes()
{
    for (MachineFamily* family : m_machineFamilies) {
        if (family->allowedResources.size() == 0) {
            qWarning() << "ERROR: NO ALLOWED RESOURCES FOUND FOR " << family->familyName;
            continue;
        }
        for (Resource* allowedResource : family->allowedResources) {
            ExtractionRecipe* r = new ExtractionRecipe();
            r->recipeClass = family->familyName + "_" + allowedResource->itemClass;
            r->recipeName = allowedResource->itemName;
            r->family = family;
            r->resource = allowedResource;
            r->outputs.insert(allowedResource, 1); // FIX: 1 is not right here
            m_extractionRecipes.insert(r->recipeClass, r);
            m_recipes.insert(r->recipeClass, r);
            allowedResource->producedBy.append(r);
        }
    }
}

const Item* GameLibrary::getItem(const QString& name) const
{
    return m_items.value(name, nullptr);
}

const Machine* GameLibrary::getMachine(const QString& name) const
{
    return m_machines.value(name, nullptr);
}

const Recipe* GameLibrary::getRecipeByClass(const QString& name) const
{
    return m_recipes.value(name, nullptr);
}

const ExtractionRecipe* GameLibrary::getExtRecipeByResource(const Resource* resource) const
{
    for (auto* recipe : resource->producedBy) {
        if (auto* p = dynamic_cast<ExtractionRecipe*>(recipe))
            return p;
    }
    qWarning() << "No Extraction Recipe Found for " << resource->itemClass;
    return nullptr;
}

const ExtractionRecipe* GameLibrary::getExtRecipeByResource(const QString& name) const
{
    const Resource* resource = m_resources.value(name);
    if (resource)
        return getExtRecipeByResource(resource);
    return nullptr;
}
const ExtractionRecipe* GameLibrary::getExtRecipeByClass(const QString& rClass) const
{
    return m_extractionRecipes.value(rClass);
}

const QMap<QString, Item*>& GameLibrary::Items() const
{
    return m_items;
}

const QMap<QString, Machine*>& GameLibrary::Machines() const
{
    return m_machines;
}

const QMap<QString, Recipe*>& GameLibrary::Recipes() const
{
    return m_recipes;
}
