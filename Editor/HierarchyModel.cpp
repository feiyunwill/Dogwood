#include "HierarchyModel.h"

#include <QtWidgets>
#include <string>

using std::string;

HierarchyModel::HierarchyModel(GameObject* root, QObject *parent)
: m_rootItem(root),
  QAbstractItemModel(parent)
{
    m_rootItem = new GameObject("Game Object Hierarchy");
    root->SetParent(m_rootItem);
}

HierarchyModel::~HierarchyModel()
{
    delete m_rootItem;
}

QModelIndex HierarchyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    GameObject* parentItem = getItem(parent);

    GameObject* childItem = parentItem->GetChild(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex HierarchyModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    GameObject *childItem = getItem(index);
    GameObject *parentItem = childItem->GetParent();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->GetChildNumber(), 0, parentItem);
}

QVariant HierarchyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    GameObject *item = getItem(index);
    return QVariant(item->GetName().c_str());
}

QVariant HierarchyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return QVariant(m_rootItem->GetName().c_str());

    return QVariant();
}

bool HierarchyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    GameObject *item = getItem(index);
    item->SetName(value.toString().toStdString());

    emit dataChanged(index, index);     // TODO do we need to test a return value here?

    return true;
}

Qt::ItemFlags HierarchyModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

int HierarchyModel::columnCount(const QModelIndex & /* parent */) const
{
    return 1;
}

int HierarchyModel::rowCount(const QModelIndex &parent) const
{
    GameObject *parentItem = getItem(parent);
    return parentItem->GetChildCount();
}

bool HierarchyModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    GameObject *parentItem = getItem(parent);

    beginInsertRows(parent, position, position + rows - 1);
    bool success = parentItem->InsertChildren(position, rows);
    endInsertRows();

    return success;
}

bool HierarchyModel::insertChild(QModelIndex parentIndex, GameObject* parent, GameObject* child, int position)
{
    beginInsertRows(parentIndex, position, position);
    bool success = parent->InsertChild(position, child);
    endInsertRows();

    return success;
}

bool HierarchyModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    GameObject *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->RemoveChildren(position, rows);
    endRemoveRows();

    return success;
}

GameObject* HierarchyModel::getItem(const QModelIndex &index) const
{
    if (index.isValid())
    {
        GameObject *item = static_cast<GameObject*>(index.internalPointer());
        if (item)
            return item;
    }
    return m_rootItem;
}