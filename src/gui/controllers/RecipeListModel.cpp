#include "RecipeListModel.h"

RecipeListModel::RecipeListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    m_gameLibrary = &GameLibrary::get();

    for (Recipe* recipe : m_gameLibrary->Recipes()) {
        m_recipes.append(recipe);
    }
}

int RecipeListModel::rowCount(const QModelIndex& parent) const
{
    return m_recipes.count();
}

QVariant RecipeListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_recipes.count())
        return QVariant();

    Recipe* entry = m_recipes.at(index.row());
    bool isExtraction = dynamic_cast<ExtractionRecipe*>(entry);

    switch (role) {
    case NameRole:
        return entry->recipeName;
    case ClassRole:
        return entry->recipeClass;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> RecipeListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ClassRole] = "class";
    return roles;
}
