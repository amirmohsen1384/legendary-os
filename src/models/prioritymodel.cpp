#include "prioritymodel.h"
#include "core/task.h"
#include <QColor>

qint64 PriorityModel::left(qint64 node)
{
    return node * 2 + 1;
}

qint64 PriorityModel::right(qint64 node)
{
    return node * 2 + 2;
}

bool PriorityModel::hasLeft(qint64 node)
{
    return left(node) < container.size();
}

qint64 PriorityModel::ancestor(qint64 node)
{
    return (node - 1) / 2;
}

bool PriorityModel::hasRight(qint64 node)
{
    return right(node) < container.size();
}

void PriorityModel::swap(qint64 one, qint64 two)
{
    if (one < 0 || one >= container.size())
    {
        return;
    }
    else if (two < 0 || two >= container.size())
    {
        return;
    }
    else
    {
        std::swap(container[one], container[two]);
        emit dataChanged(
            index(one, 0),
            index(one, columnCount() - 1),
            { Qt::DisplayRole }
            );
        emit dataChanged(
            index(two, 0),
            index(two, columnCount() - 1),
            { Qt::DisplayRole }
            );
    }
}

PriorityModel::PriorityModel(QObject *parent) : QAbstractTableModel{parent} {}

QVariant PriorityModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    auto target = container.at(index.row());
    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (index.column())
        {
        case Header::PID:
        {
            bool ok = false;
            auto result = target.data(Task::PIDRole).toLongLong(&ok);
            return ok ? result : QVariant();
        }
        case Name:
        {
            return target.data(Task::NameRole).toString();
        }
        case Priority:
        {
            bool ok = false;
            auto result = target.data(Task::PriorityRole).toLongLong(&ok);
            if (ok) {
                return QString("%1%").arg(result);
            } else {
                return {};
            }
        }
        case Agent:
        {
            auto result = target.data(Task::AgentRole).toString();
            return result.isEmpty() ? "No Agent Provided" : result;
        }
        }
    }
    case Qt::TextAlignmentRole:
    {
        return Qt::AlignCenter;
    }
    case Qt::UserRole:
    {
        return QVariant::fromValue(target);
    }
    default:
    {
        return {};
    }
    }
}

bool PriorityModel::betterThan(const QPersistentModelIndex &one, const QPersistentModelIndex &two) const
{
    auto first = one.data(Task::PriorityRole).toLongLong();
    auto second = two.data(Task::PriorityRole).toLongLong();
    return first > second;
}

void PriorityModel::upheap(qint64 node)
{
    while (node > 0)
    {
        qint64 parent = ancestor(node);
        if (!betterThan(container[node], container[parent]))
        {
            break;
        }
        swap(node, parent);
        node = parent;
    }
}

void PriorityModel::downheap(qint64 node)
{
    while (hasLeft(node))
    {
        qint64 larger = left(node);
        if (hasRight(node))
        {
            auto rightNode = right(node);
            if (betterThan(container[rightNode], container[larger]))
            {
                larger = rightNode;
            }
        }
        qint64 parent = node, child = larger;
        if (betterThan(container[child], container[parent]))
        {
            swap(child, parent);
            node = larger;
        }
        else
        {
            break;
        }
    }
}

bool PriorityModel::insertTask(const QPersistentModelIndex &index)
{
    if (!index.isValid())
    {
        return false;
    }
    auto size = container.size();
    beginInsertRows(QModelIndex(), size, size);
    container.append(QPersistentModelIndex(index));
    endInsertRows();
    upheap(size);
    return true;
}

QPersistentModelIndex PriorityModel::toTask(const QPersistentModelIndex &index) const
{
    if (!index.isValid())
    {
        return {};
    }
    return container.at(index.row());
}

void PriorityModel::removeBest()
{
    removeRow(0);
}

QPersistentModelIndex PriorityModel::peekBest() const
{
    if (container.isEmpty())
    {
        return {};
    }
    return container.constFirst();
}

void PriorityModel::clear()
{
    beginResetModel();
    container.clear();
    endResetModel();
}

int PriorityModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return container.size();
}

int PriorityModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    auto count = 0;

    // PID
    count++;

    // Name
    count++;

    // Priority
    count++;

    // Agent
    count++;

    return count;
}

QVariant PriorityModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (orientation)
    {
    case Qt::Vertical:
    {
        switch (role)
        {
        case Qt::DisplayRole:
        {
            return section + 1;
        }
        case Qt::TextAlignmentRole:
        {
            return Qt::AlignCenter;
        }
        case Qt::BackgroundRole:
        {
            return QColor(220, 220, 220);
        }
        }
    }
    case Qt::Horizontal:
    {
        switch (role)
        {
        case Qt::DisplayRole:
        {
            auto group = static_cast<Header>(section);
            switch (group)
            {
            case Header::PID:
            {
                return "PID";
            }
            case Header::Name:
            {
                return "Name";
            }
            case Header::Agent:
            {
                return "Agent";
            }
            case Header::Priority:
            {
                return "Priority";
            }
            default:
            {
                return {};
            }
            }
        }
        case Qt::TextAlignmentRole:
        {
            return Qt::AlignCenter;
        }
        case Qt::BackgroundRole:
        {
            return QColor(220, 220, 220);
        }
        }
    }
    default:
    {
        return {};
    }
    }
}

bool PriorityModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid() || row < 0 || row + count > container.size())
    {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    int last = container.size() - 1;
    for (int i = 0; i < count; ++i)
    {
        container[row + i] = container[last - i];
    }
    container.resize(container.size() - count);
    endRemoveRows();

    if (!container.isEmpty())
    {
        downheap(row);
    }
    return true;
}

