#pragma once

// #include "SceneManager.h"
#include <QAbstractListModel>
#include <QObject>
#include <QVariantList>


class Factory;
class SceneManager;
class AbstractNode;
class SceneModel : public QAbstractListModel {
    Q_OBJECT;

private:
    SceneManager* m_sceneManager = nullptr;
    QList<AbstractNode*> m_items;

public:
    explicit SceneModel(SceneManager* scene, QObject* parent = nullptr);

    enum Roles {
        NodeDataRole = Qt::UserRole + 1
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addNode(AbstractNode* node);
    void removeNode(AbstractNode* node);

public slots:
    void loadFromFactory(Factory* factory);
};
