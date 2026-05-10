#pragma once

#include "core/types/Types.h"
#include <QAbstractListModel>
#include <QObject>
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
        OutputsRole
    };
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
};
