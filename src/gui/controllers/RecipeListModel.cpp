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
    case InputsRole: {
        QVariantList list;
        if (auto* pr = dynamic_cast<ProductionRecipe*>(entry)) {
            for (auto it = pr->inputs.begin(); it != pr->inputs.end(); ++it) {
                QVariantMap map;
                map["name"] = it->first->itemName;
                map["iconUrl"] = "image://assets/item/" + it->first->itemClass;
                list.append(map);
            }
        }
        return list;
    }
    case OutputsRole: {
        QVariantList list;
        for (auto it = entry->outputs.begin(); it != entry->outputs.end(); ++it) {
            QVariantMap map;
            map["name"] = it->first->itemName;
            map["iconUrl"] = "image://assets/item/" + it->first->itemClass;
            list.append(map);
        }
        return list;
    }
    case AlternateRole: {
        if (isExtraction)
            return false;
        return static_cast<ProductionRecipe*>(entry)->isAlternate;
    }
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> RecipeListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ClassRole] = "className";
    roles[InputsRole] = "inputs";
    roles[OutputsRole] = "outputs";
    roles[AlternateRole] = "isAlternate";
    return roles;
}

// ---------------- RecipeFilterModel ----------------------
RecipeFilterModel::RecipeFilterModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

bool RecipeFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    if (m_port != nullptr) {

    }

    if (m_filterText.isEmpty())
        return true;

    auto get = [&](int role) {
        return sourceModel()->data(sourceModel()->index(sourceRow, 0, sourceParent), role);
    };

    auto itemNamesMatch = [&](const QVariant& roleData) {
        for (const QVariant& v : roleData.toList()) {
            if (v.toMap().value("name").toString().contains(m_filterText, Qt::CaseInsensitive))
                return true;
        }
        return false;
    };

    if (m_searchRecipeName && get(RecipeListModel::NameRole).toString().contains(m_filterText, Qt::CaseInsensitive))
        return true;
    if (m_mode != Outputs && itemNamesMatch(get(RecipeListModel::InputsRole)))
        return true;
    if (m_mode != Inputs && itemNamesMatch(get(RecipeListModel::OutputsRole)))
        return true;

    return false;
}

void RecipeFilterModel::setFilterString(const QString& text)
{
    m_filterText = text;
    refilter();
}

void RecipeFilterModel::setFilterMode(int mode)
{
    m_mode = static_cast<FilterIO>(mode);
    refilter();
}

void RecipeFilterModel::setSearchRecipeName(bool enabled)
{
    m_searchRecipeName = enabled;
    refilter();
}
