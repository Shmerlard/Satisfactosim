#include "SceneModel.h"
#include "core/nodes/Factory.h"
#include "core/nodes/AbstractNode.h"

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

    AbstractNode* entry = m_items.at(index.row());

    switch (role) {
    case NodeDataRole:
        return QVariant::fromValue<QObject*>(entry);
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
        m_items.append(node.get());
    }
    endResetModel();
}

void SceneModel::addNode(AbstractNode* node)
{
    int row = m_items.count();
    beginInsertRows(QModelIndex(), row, row);
    m_items.append(node);
    endInsertRows();
}

void SceneModel::removeNode(AbstractNode* node)
{
    int row = m_items.indexOf(node);
    if (row < 0)
        return;
    beginRemoveRows(QModelIndex(), row, row);
    m_items.removeAt(row);
    endRemoveRows();
}
