#include "SessionManager.h"
#include "GameLibrary.h"
#include "core/nodes/ProductionNode.h"

ProductionNode* SessionManager::createProductionNode(const Recipe& recipe, Factory* factory, QString name)
{
    Factory* f = factory ? factory : m_activeFactory;
    ProductionNode* node = new ProductionNode(*f, recipe, name);
    emit nodeAdded(node);
    return node;
}
ProductionNode* SessionManager::createProductionNodeByClass(const QString& rClass, Factory* factory, QString name)
{
    const Recipe* r = GameLibrary::get().getRecipeByClass(rClass);
    if (!r)
        return nullptr;

    return createProductionNode(*r, factory, name);
}

ExtractionNode* SessionManager::createExtractionNode(const ExtractionRecipe* recipe, int tier, Factory* factory, QString name)
{
    Factory* f = factory ? factory : m_activeFactory;
    if (!recipe)
        return nullptr;
    ExtractionNode* node = new ExtractionNode(*f, *recipe, tier, name);
    emit nodeAdded(node);
    return node;
}

ExtractionNode* SessionManager::createExtractionNodeByName(QString resourceName, int tier, Factory* factory, QString name)
{
    const ExtractionRecipe* r = GameLibrary::get().getExtRecipeByResource(resourceName);
    if (!r)
        return nullptr;
    return createExtractionNode(r, tier, factory, name);
}

Factory* SessionManager::createFactory(Factory* parent, QString name)
{
    Factory* p = parent ? parent : m_activeFactory;
    Factory* factory = new Factory(p, name);
    FactoryNode* node = new FactoryNode(*p, *factory, name);
    emit nodeAdded(node);
    return factory;
}
void SessionManager::enterFactory(Factory* f)
{
    if (!f)
        return;
    m_activeFactory = f;
    emit factoryChanged(f);
    // emit activeFactoryChanged();
}
