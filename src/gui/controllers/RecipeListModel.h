#pragma once

#include "core/types/Types.h"
#include <QAbstractListModel>
#include <QObject>
#include <QSortFilterProxyModel>
#include <core/managers/GameLibrary.h>

class RecipeListModel : public QAbstractListModel {
    Q_OBJECT;

private:
    GameLibrary* m_gameLibrary = nullptr;
    QList<Recipe*> m_recipes;

public:
    explicit RecipeListModel(QObject* parent = nullptr);
    enum Roles {
        NameRole = Qt::UserRole + 1,
        ClassRole,
        InputsRole,
        OutputsRole,
        AlternateRole

    };
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
};

class RecipeFilterModel : public QSortFilterProxyModel {
    Q_OBJECT;

public:
    enum FilterMode { All,
        Inputs,
        Outputs };

    explicit RecipeFilterModel(QObject* parent = nullptr)
        : QSortFilterProxyModel(parent)
    {
        setFilterCaseSensitivity(Qt::CaseInsensitive);
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    QString m_filterText;
    FilterMode m_mode = All;
    bool m_searchRecipeName = true;

    void refilter() { invalidateFilter(); }

public slots:
    void setFilterString(const QString& text)
    {
        m_filterText = text;
        refilter();
    }
    void setFilterMode(int mode)
    {
        m_mode = static_cast<FilterMode>(mode);
        refilter();
    }
    void setSearchRecipeName(bool enabled)
    {
        m_searchRecipeName = enabled;
        refilter();
    }
};
