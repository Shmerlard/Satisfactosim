#include "SceneModel.h"

SceneModel::SceneModel(SceneManager* scene, QObject* parent)
    : QAbstractListModel(parent)
    , m_sceneManager(scene)
{
}

int SceneModel::rowCount(const QModelIndex& parent) const
{
    return m_items.count();
}

QVariant SceneModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.count())
        return QVariant();

    AbstractItem* entry = m_items.at(index.row());

    switch (role) {
    case NodeDataRole:
        return QVariant::fromValue(entry);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> SceneModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NodeDataRole] = "nodeData";
    return roles;
}

void SceneModel::loadFromFactory(Factory* factory)
{
    if (!factory)
        return;

    beginResetModel();
    m_items.clear();
    for (const auto& node : factory->subNodes()) {
        AbstractItem* item = m_sceneManager->itemFromNode(node.get());
        if (item)
            m_items.append(item);
    }
    endResetModel();
}
