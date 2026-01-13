#include "readytaskmodel.h"
#include "core/task.h"

qint64 ReadyTaskModel::left(qint64 node)
{
    return node * 2 + 1;
}

qint64 ReadyTaskModel::right(qint64 node)
{
    return node * 2 + 2;
}

bool ReadyTaskModel::hasLeft(qint64 node)
{
    return left(node) < container.size();
}

qint64 ReadyTaskModel::ancestor(qint64 node)
{
    return (node - 1) / 2;
}

bool ReadyTaskModel::hasRight(qint64 node)
{
    return right(node) < container.size();
}

void ReadyTaskModel::swap(qint64 one, qint64 two)
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
        emit dataChanged(index(one, 0), index(one, columnCount() - 1), {Qt::DisplayRole});
        emit dataChanged(index(two, 0), index(two, columnCount() - 1), {Qt::DisplayRole});
    }
}

void ReadyTaskModel::upheap(qint64 node)
{
    qint64 top = ancestor(node);
    while (top >= 0)
    {
        const auto topPriority = container[top].data(Task::PriorityRole).toLongLong();
        const auto nodePriority = container[node].data(Task::PriorityRole).toLongLong();
        if (topPriority < nodePriority)
        {
            swap(top, node);
            node = top;
            top = ancestor(node);
        }
        else
        {
            break;
        }
    }
}

void ReadyTaskModel::downheap(qint64 node)
{
    while (hasLeft(node))
    {
        qint64 larger = left(node);
        if (hasRight(node))
        {
            auto rightNode = right(node);
            const auto rightPriority = container[rightNode].data(Task::PriorityRole).toLongLong();
            const auto largerPriority = container[larger].data(Task::PriorityRole).toLongLong();
            if (largerPriority < rightPriority)
            {
                larger = rightNode;
            }
        }
        qint64 parent = node;
        qint64 child = larger;
        const auto childPriority = container[child].data(Task::PriorityRole).toLongLong();
        const auto parentPriority = container[parent].data(Task::PriorityRole).toLongLong();
        if (parentPriority < childPriority)
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

ReadyTaskModel::ReadyTaskModel(qsizetype maximum, QObject *parent) : QAbstractListModel(parent)
{
}

QVariant ReadyTaskModel::headerData(int section, Qt::Orientation orientation, int role) const
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
    case Qt::Horizontal:
    {
        switch(role)
        {
        case Qt::DisplayRole:
        {
            auto group = static_cast<Header>(section);
            switch (group)
            {
            case Header::Name:
            {
                return "Name";
            }
            case Header::PID:
            {
                return "PID";
            }
            case Header::Priority:
            {
                return "Priority";
            }
            case Header::Progress:
            {
                return "Progress";
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
        }
    }
    }
}

qsizetype ReadyTaskModel::getMaximumSize() const
{
    return maximum;
}

bool ReadyTaskModel::insertTask(const QPersistentModelIndex &index)
{
    auto size = container.size();
    if (size >= maximum)
    {
        return false;
    }
    beginInsertRows(QModelIndex(), size, size);
    container.append(index);
    endInsertRows();
    upheap(size);
    return true;
}

void ReadyTaskModel::removeMostCritical()
{
    if (!container.isEmpty())
    {
        auto size = container.size();
        beginRemoveRows(QModelIndex(), size - 1, size - 1);
        auto data = container.takeAt(size - 1);
        endRemoveRows();
        if (!container.isEmpty())
        {
            container[0] = data;
            downheap(0);
        }
    }
}

QPersistentModelIndex ReadyTaskModel::getMostCritical() const
{
    return container.front();
}


void ReadyTaskModel::setMaximumSize(qsizetype size)
{
    maximum = size;
    container.reserve(size);
}

int ReadyTaskModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
    {
        return 0;
    }
    return container.size();
}

int ReadyTaskModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
    {
        return 0;
    }

    auto count = 0;

    // Name
    count++;

    // PID
    count++;

    // Progress
    count++;

    // Priority
    count++;

    return count;
}

QVariant ReadyTaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return {};
    }
    auto target = container.at(index.row());
    switch (role)
    {
    case Qt::DisplayRole:
    {
        auto group = static_cast<Header>(index.column());
        switch (group)
        {
        case Header::Name:
        {
            return target.data(Task::NameRole).toString();
        }
        case Header::PID:
        {
            return target.data(Task::PIDRole).toLongLong();
        }
        case Header::Priority:
        {
            auto priority = target.data(Task::PriorityRole).toLongLong();
            return QString("%1%").arg(priority);
        }
        case Header::Progress:
        {
            auto burst = target.data(Task::BurstTimeRole).toLongLong();
            auto remaining = target.data(Task::RemainingTimeRole).toLongLong();
            return QString("%1%").arg((1 - remaining / burst) * 100);
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
    }
}
