#pragma once

#include "SceneManager.h"
#include <QAbstractListModel>
#include <QObject>
#include <QVariantList>

class SceneModel : public QAbstractListModel {
    Q_OBJECT;

private:
    SceneManager* m_sceneManager = nullptr;
    // QMap<AbstractNode*, AbstractItem*> m_nodeItemMap;
    QList<AbstractItem*> m_items;

public:
    explicit SceneModel(SceneManager* scene, QObject* parent = nullptr);

    enum Roles {
        NodeDataRole = Qt::UserRole + 1
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void loadFromFactory(Factory* factory);
};
