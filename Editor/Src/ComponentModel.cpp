#include "ComponentModel.h"
#include "ComponentModelItem.h"
#include "DebugLogger.h"
#include "GameObjectReference.h"
#include "ToolsideGameComponent.h"
#include "ToolsideGameObject.h"

#include <QtWidgets>
#include <string>
#include <EditorCommands.h>

using std::string;
using namespace EditorCommands;

ComponentModel::ComponentModel(QObject *parent, ToolsideGameObject* go)
    : m_gameObject(go), QAbstractItemModel(parent)
{
    BuildModel();
}

void ComponentModel::BuildModel()
{
    m_rootItem = new ComponentModelItem("ROOT");

    AddTransformData();
    AddMeshData();
    AddColliderData();
    AddRigidBodyData();
    AddComponentData();

    emit layoutChanged();
}

void ComponentModel::RefreshModel()
{
    ClearModel();
    BuildModel();

    emit layoutChanged();
}

void ComponentModel::ClearModel()
{
    delete m_rootItem;      // The item destructor will take care of deleting child items recursively
}

ComponentModelItem* ComponentModel::GetItem(const QModelIndex &index) const
{
    if (index.isValid())
    {
        ComponentModelItem *item = static_cast<ComponentModelItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return m_rootItem;
}

QModelIndex ComponentModel::index(int row, int column, const QModelIndex& parent) const
{
    ComponentModelItem* parentItem = GetItem(parent);
    ComponentModelItem* childItem = parentItem->GetChild(row);

    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex ComponentModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ComponentModelItem* childItem = GetItem(index);
    ComponentModelItem* parentItem = childItem->GetParent();

    if (parentItem == m_rootItem || parentItem == NULL)
        return QModelIndex();

    return createIndex(parentItem->GetIndexInParent(), 0, parentItem);
}

int ComponentModel::rowCount(const QModelIndex& index) const
{
    ComponentModelItem* item = GetItem(index);
    return item->GetChildCount();
}

int ComponentModel::columnCount(const QModelIndex& /*index*/) const
{
    return 2;
}

QVariant ComponentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int col = index.column();
    ComponentModelItem *item = GetItem(index);
    return item->GetData((ComponentModelItem::ColumnType)col, role);
}

bool ComponentModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if ((role != Qt::EditRole && role != Qt::CheckStateRole) || !IsEditable(index))
        return false;

    ComponentModelItem* item = GetItem(index);
    return item->SetData(value);
}

Qt::ItemFlags ComponentModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (index.isValid() && IsEditable(index))
    {
        return  Qt::ItemIsEditable | Qt::ItemIsDropEnabled | Qt::ItemIsUserCheckable | defaultFlags;
    }
    else
    {
        return Qt::ItemIsDropEnabled | defaultFlags;
    }
}

Qt::DropActions ComponentModel::supportedDragActions() const
{
    return Qt::IgnoreAction;
}

// TODO use link action only?
Qt::DropActions ComponentModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

QStringList ComponentModel::mimeTypes() const
{
    QStringList types;
    types << "DogwoodEngine/GameObject" << "DogwoodEngine/AssetInfo";
    return types;
}

bool ComponentModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int /*row*/, int /*column*/, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (!parent.isValid())
        return false;

    ComponentModelItem* item = GetItem(parent);
    bool success = item->DropData(data);

    ClearModel();
    BuildModel();

    return success;
}

bool ComponentModel::IsEditable(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;

    int col = index.column();
    ComponentModelItem *item = GetItem(index);
    if (col == 0 || !item->IsEditable())
        return false;

    return true;
}

void ComponentModel::AddTransformData()
{
    ComponentModelTransformItem* transformItem = new ComponentModelTransformItem(m_gameObject);
    m_rootItem->AddChild(transformItem);
}

void ComponentModel::AddMeshData()
{
    MeshInstance* meshInstance = m_gameObject->GetMeshInstance();
    if (meshInstance == NULL)
        return;

    ComponentModelMeshItem* meshItem = new ComponentModelMeshItem(meshInstance);
    m_rootItem->AddChild(meshItem);
}

void ComponentModel::AddColliderData()
{
    vector<Collider*> colliderList = m_gameObject->GetColliders();

    // Add data for each collider on the game object
    vector<Collider*>::iterator iter = colliderList.begin();
    for (; iter != colliderList.end(); iter++)
    {
        // Add header for this script
        Collider* collider = *iter;
        ComponentModelColliderItem* headerItem = new ComponentModelColliderItem(collider);
        m_rootItem->AddChild(headerItem);
    }
}

void ComponentModel::AddRigidBodyData()
{
    RigidBody* rigidBody = m_gameObject->GetRigidBody();
    if (rigidBody == NULL)
        return;

    ComponentModelRigidBodyItem* rigidBodyItem = new ComponentModelRigidBodyItem(rigidBody);
    m_rootItem->AddChild(rigidBodyItem);
}

void ComponentModel::AddComponentData()
{
    vector<ToolsideGameComponent*> componentList = m_gameObject->GetComponentList();

    // Add data for each script component on the game object
    vector<ToolsideGameComponent*>::iterator iter = componentList.begin();
    for (; iter != componentList.end(); iter++)
    {
        // Add header for this script
        ToolsideGameComponent* component = *iter;
        ComponentModelScriptItem* headerItem = new ComponentModelScriptItem(component);
        m_rootItem->AddChild(headerItem);
    }
}