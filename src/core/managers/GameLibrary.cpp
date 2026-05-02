#include "GameLibrary.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

namespace {
Item* itemFromJsonObject(const QJsonValue& value)
{

    QJsonObject obj = value.toObject();
    Item* i = new Item();

    i->itemClass = obj["Class"].toString();
    i->itemName = obj["DisplayName"].toString();
    i->sinkPoints = obj["SinkPoints"].toString().toDouble();
    i->form = obj["Form"].toString();
    i->iconPath = QString("assets/icons/items/%1.png").arg(i->itemClass);
    return i;
}

Machine* machineFromJsonObject(const QJsonValue& value)
{

    QJsonObject obj = value.toObject();
    Machine* m = new Machine();

    m->machineClass = obj["Class"].toString();
    m->machineName = obj["DisplayName"].toString();
    m->powerConsumption = obj["PowerConsumption"].toString().toDouble();
    m->iconPath = QString("assets/icons/machines/%1.png").arg(m->machineName);
    return m;
}

} // namespace

GameLibrary::~GameLibrary()
{
    qDeleteAll(m_items);
    qDeleteAll(m_machines);
    qDeleteAll(m_recipes);
    qDeleteAll(m_extractionRecipes);
    qDeleteAll(m_machineFamilies);
}

void GameLibrary::loadData()
{
    parseItems();
    parseMachines();
    parseRecipes();
    connectRecipes();

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

    for (const QJsonValue& value : root["Items"].toArray()) {
        Item* i = itemFromJsonObject(value);
        i->isResource = false;
        m_items.insert(i->itemClass, i);
    }
    for (const QJsonValue& value : root["Resources"].toArray()) {
        Item* i = itemFromJsonObject(value);

        i->isResource = true;
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
        Machine* m = machineFromJsonObject(value);

        m->isExtractor = false;
        m_machines.insert(m->machineClass, m);
    }

    for (const QJsonValue& value : root["ExtractionMachines"].toArray()) {
        QJsonObject obj = value.toObject();
        Machine* m = machineFromJsonObject(value);
        QString extractorType = obj["ExtractorTypeName"].toString() == "None"
            ? obj["Class"].toString()
            : obj["ExtractorTypeName"].toString();

        m->isExtractor = true;
        ExtractorSettings* es = new ExtractorSettings();
        m->extractorSettings = es;
        // es->extractorType = obj["ExtractorTypeName"].toString();
        es->extractorType = extractorType;
        es->extractCycleTime = obj["ExtractCycleTime"].toString().toDouble();
        es->itemsPerCycle = obj["ItemsPerCycle"].toString().toDouble();

        if (obj["OnlyAllowCertainResources"].toBool()) {
            QJsonArray allowedResources = obj["AllowedResources"].toArray();
            for (const QJsonValue& res : allowedResources) {
                QString resClass = res.toString();
                Item* resItem = m_items.value(resClass);
                if (resItem) {
                    es->allowedResources.append(resItem);
                    // qDebug() << resClass;
                }
            }
        } else {
            QJsonArray allowedFormsArr = obj["AllowedResourceForms"].toArray();
            for (const QJsonValue& formVal : allowedFormsArr) {
                QString form = formVal.toString();
                for (Item* resource : m_resources) {
                    if (resource && resource->form == form) {
                        es->allowedResources.append(resource);
                    }
                }
            }
        }

        if (!m_machineFamilies.contains(es->extractorType)) {
            MachineFamily* family = new MachineFamily();
            family->familyName = es->extractorType;
            m_machineFamilies.insert(es->extractorType, family);
        }
        m_machineFamilies[es->extractorType]->tiers.append(m);

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
        Recipe* r = new Recipe();

        r->recipeClass = obj["Class"].toString();
        r->recipeName = obj["DisplayName"].toString();
        r->isAlternate = obj["Alternate"].toBool();
        r->recipeTime = obj["ManufactoringDuration"].toString().toDouble();

        m_recipes.insert(r->recipeClass, r);
    }
}

void GameLibrary::connectRecipes()
{
    QFile recipesFile(":/assets/jsons/recipes.json");
    if (!recipesFile.open(QIODevice::ReadOnly))
        return;
    QJsonDocument doc = QJsonDocument::fromJson(recipesFile.readAll());
    QJsonArray rootArray = doc.array();

    for (const QJsonValue& value : rootArray) {
        QJsonObject obj = value.toObject();
        QString recipeClass = obj["Class"].toString();
        Recipe* r = m_recipes.value(recipeClass);

        if (!r)
            continue;

        QString machineClass = obj["ProducedIn"].toString();
        Machine* machine = m_machines.value(machineClass);

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
                if (item->form == "RF_LIQUID" || item->form == "RF_GAS")
                    ingAmount /= 1000;
                r->inputs.insert(item, ingAmount);
                item->usedIn.append(r);
            }
        }
        QJsonArray prodArr = obj["Product"].toArray();
        for (const QJsonValue& prod : prodArr) {
            QJsonObject prodObj = prod.toObject();
            QString prodClass = prodObj["Item"].toString();
            Item* item = m_items.value(prodClass);
            float prodAmount = prodObj["Amount"].toDouble();
            if (item) {
                if (item->form == "RF_LIQUID" || item->form == "RF_GAS")
                    prodAmount /= 1000;

                r->outputs.insert(item, prodAmount);
                item->producedBy.append(r);
            }
        }
    }
}

void GameLibrary::parseExtractionRecipes()
{
    for (MachineFamily* family : m_machineFamilies) {
        ExtractorSettings* es = family->tiers.first()->extractorSettings;
        for (Item* allowedResource : es->allowedResources) {
            ExtractionRecipe* r = new ExtractionRecipe();
            r->recipeClass = family->familyName + "_" + allowedResource->itemClass;
            r->recipeName = family->familyName + " → " + allowedResource->itemName;
            r->family = family;
            r->resource = allowedResource;
            m_extractionRecipes.insert(r->recipeClass, r);
            allowedResource->extractedBy.append(r);
            es->extractionRecipes.append(r);
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

const ExtractionRecipe* GameLibrary::getExtRecipeByResource(const Item* resource)
{
    if (resource->extractedBy.count() > 1)
        qDebug() << "MORE THAN 1 RECIPES FOUND FOR " << resource->itemClass;
    if (resource->extractedBy.count() == 0) {
        qDebug() << "No extractors found for " << resource->itemClass;
        return nullptr;
    }
    return resource->extractedBy.first();
}

const ExtractionRecipe* GameLibrary::getExtRecipeByResource(const QString name)
{
    const Item* resource = m_resources.value(name);
    if (resource)
        return getExtRecipeByResource(resource);
    return nullptr;
}
const QMap<QString, Item*> GameLibrary::Items() const
{
    return m_items;
}
const QMap<QString, Machine*> GameLibrary::Machines() const
{
    return m_machines;
}
const QMap<QString, Recipe*> GameLibrary::Recipes() const
{
    return m_recipes;
}
