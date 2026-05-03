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

Item* itemFromJsonObject(const QJsonValue& value)
{

    QJsonObject obj = value.toObject();
    bool isResource = obj["isResource"].toBool();
    Item* i = nullptr;
    if (isResource)
        i = new Resource();
    else
        i = new Component();

    i->itemClass = obj["Class"].toString();
    i->itemName = obj["DisplayName"].toString();
    i->sinkPoints = obj["SinkPoints"].toString().toDouble();
    i->form = formFromString(obj["Form"].toString());
    i->iconPath = QString("assets/icons/items/%1.png").arg(i->itemClass);
    // TODO: energy

    return i;
}

// ProductionMachine* machineFromJsonObject(const QJsonValue& value)
// {
//     QJsonObject obj = value.toObject();
//     Item* m = nullptr;
//
//     m->machineClass = obj["Class"].toString();
//     m->machineName = obj["DisplayName"].toString();
//     m->basePowerConsumption = obj["PowerConsumption"].toString().toDouble();
//     m->iconPath = QString("assets/icons/machines/%1.png").arg(m->machineName);
//
//     return m;
// }

/**
 * initializes the machine object from json
 * also initializes the family (name only)
 */
ProductionMachine* productionMachineFromJsonObject(const QJsonValue& value)
{
    // FIX: NEEDS A CHECK
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
    // FIX: NEEDS A CHECK
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
    // TODO: m->tier is never set — ExtractionMachine::tier stays uninitialized (garbage int)
    // m->tier =
    m->extractCycleTime = obj["ExtractCycleTime"].toDouble();
    m->itemsPerCycle = obj["ItemsPerCycle"].toInt();
    return m;
}

// Machine* machineFromJsonObject(const QJsonValue& value, QMap<QString, MachineFamily*>& families)
// {
//     // FIX: NEEDS A CHECK
//     QJsonObject obj = value.toObject();
//     Machine* m = nullptr;
//
//     QString _machineType = obj["_MachineType"].toString();
//     if (_machineType == "Production")
//         m = new ProductionMachine();
//     else if (_machineType == "Extraction")
//         m = new ProductionMachine();
//     else {
//         qWarning() << "Unknown Type: " << _machineType;
//         return nullptr;
//     }
//
//     m->machineClass = obj["Class"].toString();
//     m->machineName = obj["DisplayName"].toString();
//     m->basePowerConsumption = obj["PowerConsumption"].toString().toDouble();
//     m->iconPath = QString("assets/icons/machines/%1.png").arg(m->machineName);
//
//     if (_machineType == "Extraction") {
//         auto* ext_m = static_cast<ExtractionMachine*>(m);
//         QString extrTypeName = obj["ExtractorTypeName"].toString();
//         if (!families.contains(extrTypeName)) {
//             MachineFamily* newFamily = new MachineFamily();
//             newFamily->familyName = extrTypeName;
//
//             families.insert(extrTypeName, newFamily);
//         }
//         MachineFamily* fam = families.value(extrTypeName);
//         fam->tiers.append(ext_m);
//     }
//     return m;
// }

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
    // connectRecipes();

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
        Item* i = itemFromJsonObject(value);
        m_items.insert(i->itemClass, i);
    }
    for (const QJsonValue& value : root["Resources"].toArray()) {
        // TODO: static_cast is unsafe here — itemFromJsonObject reads "isResource" from JSON, but items in the "Resources" array may not have that field set, causing it to create a Component* that gets cast to Resource* (UB). Pass isResource as a parameter instead.
    auto* i = static_cast<Resource*>(itemFromJsonObject(value)); // TODO: is static_cast correct
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
        m_productioMachines.insert(m->machineClass, m);
        m_machines.insert(m->machineClass, m);
    }

    for (const QJsonValue& value : root["ExtractionMachines"].toArray()) {
        QJsonObject obj = value.toObject();
        ExtractionMachine* m = extractionMachineFromJsonObject(value, m_machineFamilies);
        // TODO: extractorType is computed but never used — extractionMachineFromJsonObject already handles this internally
        QString extractorType = obj["ExtractorTypeName"].toString() == "None"
            ? obj["Class"].toString()
            : obj["ExtractorTypeName"].toString();

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
        r->recipeName = obj["DisplayName"].toString();
        r->isAlternate = obj["Alternate"].toBool();
        // TODO: verify JSON field name — "ManufactoringDuration" may be a typo of "ManufacturingDuration"
        r->recipeTime = obj["ManufactoringDuration"].toString().toDouble();

        m_recipes.insert(r->recipeClass, r);

        /////
        ProductionMachine* machine = m_productioMachines.value(obj["ProducedIn"].toString());
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

// void GameLibrary::connectRecipes()
// {
//     QFile recipesFile(":/assets/jsons/recipes.json");
//     if (!recipesFile.open(QIODevice::ReadOnly))
//         return;
//     QJsonDocument doc = QJsonDocument::fromJson(recipesFile.readAll());
//     QJsonArray rootArray = doc.array();
//
//     for (const QJsonValue& value : rootArray) {
//         QJsonObject obj = value.toObject();
//         QString recipeClass = obj["Class"].toString();
//         Recipe* r = m_recipes.value(recipeClass);
//
//         if (!r)
//             continue;
//
//         QString machineClass = obj["ProducedIn"].toString();
//         Machine* machine = m_machines.value(machineClass);
//
//         if (machine) {
//             r->producedIn = machine;
//             machine->recipes.append(r);
//         } else {
//             qWarning() << "WARN: no machine found in recipe: " << r->recipeClass;
//         }
//
//         QJsonArray ingArr = obj["Ingredients"].toArray();
//         for (const QJsonValue& ing : ingArr) {
//             QJsonObject ingObj = ing.toObject();
//             QString ingClass = ingObj["Item"].toString();
//             Item* item = m_items.value(ingClass);
//             float ingAmount = ingObj["Amount"].toDouble();
//
//             if (item) {
//                 if (item->form == "RF_LIQUID" || item->form == "RF_GAS")
//                     ingAmount /= 1000;
//                 r->inputs.insert(item, ingAmount);
//                 item->usedIn.append(r);
//             }
//         }
//         QJsonArray prodArr = obj["Product"].toArray();
//         for (const QJsonValue& prod : prodArr) {
//             QJsonObject prodObj = prod.toObject();
//             QString prodClass = prodObj["Item"].toString();
//             Item* item = m_items.value(prodClass);
//             float prodAmount = prodObj["Amount"].toDouble();
//             if (item) {
//                 if (item->form == "RF_LIQUID" || item->form == "RF_GAS")
//                     prodAmount /= 1000;
//
//                 r->outputs.insert(item, prodAmount);
//                 item->producedBy.append(r);
//             }
//         }
//     }
// }

void GameLibrary::parseExtractionRecipes()
{
    for (MachineFamily* family : m_machineFamilies) {
        // TODO: family->allowedResources is populated in parseMachines — if a family has no allowed resources, no extraction recipes are created and the family is effectively broken silently
        // ExtractorSettings* es = family->tiers.first()->extractorSettings;
        for (Resource* allowedResource : family->allowedResources) {
            ExtractionRecipe* r = new ExtractionRecipe();
            r->recipeClass = family->familyName + "_" + allowedResource->itemClass;
            r->recipeName = allowedResource->itemName;
            r->family = family;
            r->resource = allowedResource;
            m_extractionRecipes.insert(r->recipeClass, r);
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

// const ProductionRecipe* GameLibrary::getProductionRecipe(const Item* resource)
// {
//     
// }
// const ProductionRecipe* GameLibrary::getProductionRecipe(const QString name)
// {
// }
const ExtractionRecipe* GameLibrary::getExtRecipeByResource(const Resource* resource)
{
    // if (resource->producedBy.count() > 1)
    //     qDebug() << "MORE THAN 1 RECIPES FOUND FOR " << resource->itemClass;
    // if (resource->producedBy.count() == 0) {
    //     qDebug() << "No extractors found for " << resource->itemClass;
    //     return nullptr;
    // }
    for (auto* recipe : resource->producedBy ) {
        if (auto* p = dynamic_cast<ExtractionRecipe*>(recipe))
            return p;
    }
    qWarning() << "No Extraction Recipe Found for " << resource->itemClass;
    return nullptr;
}

const ExtractionRecipe* GameLibrary::getExtRecipeByResource(const QString name)
{
    const Resource* resource = m_resources.value(name);
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
