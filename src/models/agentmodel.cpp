#include "agentmodel.h"

Agent* AgentModel::createAgent(const AgentInfo &info, Agent* parent)
{
    if (!parent)
    {
        return {};
    }
    auto agent = std::make_unique<Agent>(parent);
    agent->setDescription(info.getDescription());
    agent->setName(info.getName());

    auto result = agent.get();
    parent->addChild(std::move(agent));

    return result;
}

AgentModel::AgentModel(QObject *parent) : QAbstractItemModel(parent)
{
    root = std::make_unique<Agent>();
    root->setName("Main Root");
}

QModelIndex AgentModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return {};
    }
    auto ancestor = parent.isValid() ? static_cast<Agent*>(parent.internalPointer()) : root.get();
    if (!ancestor)
    {
        ancestor = root.get();
    }
    auto item = ancestor->getChild(row);
    return !item ? QModelIndex() : createIndex(row, column, item);
}

QModelIndex AgentModel::index(const QString &path) const
{
    QStringList tokens = path.split('/');
    QStringListIterator iterator(tokens);
    if (!iterator.hasNext())
    {
        return {};
    }
    else if (!iterator.next().isEmpty())
    {
        return {};
    }
    auto target = root.get();
    while (iterator.hasNext())
    {
        const QString &name = iterator.next();
        auto result = target->find(name);
        if (!result)
        {
            return {};
        }
        target = result;
    }
    return createIndex(target->getRow(), 0, target);
}

QModelIndex AgentModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
    {
        return {};
    }
    auto item = static_cast<Agent*>(index.internalPointer());
    if (!item)
    {
        item = root.get();
    }
    auto parent = item->getParent();
    if (parent == nullptr || parent == root.get())
    {
        return {};
    }
    else
    {
        return createIndex(parent->getRow(), 0, parent);
    }
}

int AgentModel::rowCount(const QModelIndex &parent) const
{
    auto ancestor = !parent.isValid() ? root.get() : static_cast<Agent*>(parent.internalPointer());
    return ancestor ? ancestor->childCount() : 0;
}

QVariant AgentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
    {
        return {};
    }
    switch (role)
    {
    case Qt::DisplayRole:
    {
        auto group = static_cast<AgentModel::Header>(section);
        switch(group)
        {
        case AgentModel::Header::Name:
        {
            return "Name";
        }
        case AgentModel::Header::Description:
        {
            return "Description";
        }
        default:
        {
            return {};
        }
        }
    }
    case Qt::BackgroundRole:
    {
        return QColor(220, 220, 220);
    }
    case Qt::TextAlignmentRole:
    {
        return Qt::AlignCenter;
    }
    default:
    {
        return {};
    }
    }
}

int AgentModel::columnCount(const QModelIndex &parent) const
{
    auto ancestor = !parent.isValid() ? root.get() : static_cast<Agent*>(parent.internalPointer());
    return ancestor ? ancestor->columnCount() : 0;
}

QVariant AgentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return {};
    }
    auto item = static_cast<Agent*>(index.internalPointer());
    if(!item)
    {
        return {};
    }
    switch (role)
    {
    case Qt::DisplayRole:
    {
        auto group = static_cast<AgentModel::Header>(index.column());
        switch(group)
        {
        case AgentModel::Header::Name:
        {
            return item->getName();
        }
        case AgentModel::Header::Description:
        {
            auto description = item->getDescription();
            return description;
        }
        default:
        {
            return {};
        }
        }
    }
    case Qt::ToolTipRole:
    {
        auto group = static_cast<AgentModel::Header>(index.column());
        switch(group)
        {
        case AgentModel::Header::Description:
        {
            return item->getDescription();
        }
        default:
        {
            return {};
        }
        }
    }
    case Qt::TextAlignmentRole:
    {
        return Qt::AlignVCenter;
    }
    case Agent::NameRole:
    {
        return item->getName();
    }
    case Agent::DescriptionRole:
    {
        return item->getDescription();
    }
    default:
    {
        return {};
    }
    }
}

bool AgentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto changed = false;
    if (!value.isValid())
    {
        return changed;
    }
    auto item = static_cast<Agent*>(index.internalPointer());
    switch(role)
    {
    case Agent::NameRole:
    {
        if (value.canConvert<QString>())
        {
            item->setName(value.toString());
            changed = true;
        }
        break;
    }
    case Agent::DescriptionRole:
    {
        if (value.canConvert<QString>())
        {
            item->setDescription(value.toString());
            changed = true;
        }
    }
    }
    if (changed)
    {
        emit dataChanged(index, index, {role, Qt::DisplayRole, Qt::ToolTipRole});
    }
    return changed;
}

QModelIndex AgentModel::insertAgent(const AgentInfo &info, const QModelIndex &parent)
{
    auto ancestor = !parent.isValid() ? root.get() : static_cast<Agent*>(parent.internalPointer());
    if(!ancestor)
    {
        return QModelIndex();
    }
    auto row = ancestor->childCount();

    beginInsertRows(parent, row, row);
    auto agent = createAgent(info, ancestor);
    endInsertRows();

    return createIndex(row, 0, agent);
}

bool AgentModel::removeAgent(const QModelIndex &index)
{
    if (!index.isValid())
    {
        clear();
        return true;
    }

    const auto row = index.row();
    auto parentIndex = index.parent();
    auto parent = parentIndex.isValid() ? static_cast<Agent*>(parentIndex.internalPointer()) : root.get();

    if (row < 0 || row >= parent->childCount()) {
        return false;
    }

    beginRemoveRows(parentIndex, row, row);
    auto result = parent->removeChild(row);
    endRemoveRows();

    return result;
}

void AgentModel::clear()
{
    beginResetModel();
    root.reset(nullptr);
    root = std::make_unique<Agent>(nullptr);
    root->setName("Main Root");
    endResetModel();
}

QString AgentModel::toString(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return QString();
    }
    return QString("%1/%2").arg(
        toString(index.parent()),
        index.data(Agent::NameRole).toString()
    );
}
