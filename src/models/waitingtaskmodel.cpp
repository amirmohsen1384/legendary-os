#include "waitingtaskmodel.h"
#include "core/task.h"
#include <QColor>

WaitingTaskModel::WaitingTaskModel(QObject *parent) : QAbstractTableModel(parent)
{}

int WaitingTaskModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return container.size();
}

QVariant WaitingTaskModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            case Header::Reason:
            {
                return "Wait Reason";
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

void WaitingTaskModel::insert(const QPersistentModelIndex &index)
{
    beginInsertRows(QModelIndex(), container.size(), container.size());
    container.push_back(index);
    endInsertRows();
}

void WaitingTaskModel::remove(const QPersistentModelIndex &index)
{
    for (auto row = 0; row < container.size(); ++row)
    {
        if (container.at(row) == index)
        {
            beginRemoveRows(QModelIndex(), row, row);
            container.removeAt(row);
            endRemoveRows();
            break;
        }
    }
}

int WaitingTaskModel::columnCount(const QModelIndex &parent) const
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

    // Reason
    count++;

    return count;
}

QVariant WaitingTaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return {};
    }
    const auto target = container.at(index.row());
    switch (role)
    {
    case Qt::DisplayRole:
    {
        auto group = static_cast<Header>(index.column());
        switch (group)
        {
        case Header::PID:
        {
            return target.data(Task::PIDRole).toLongLong();
        }
        case Header::Name:
        {
            return target.data(Task::NameRole).toString();
        }
        case Header::Reason:
        {
            auto state = target.data(Task::StateRole).value<Task::State>();
            switch(state)
            {
            case Task::State::WaitingForAgent:
            {
                return "Agent not available";
            }
            case Task::State::WaitingForLimit:
            {
                return "Maximum queue reached";
            }
            }
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
    case Qt::UserRole:
    {
        return QVariant::fromValue(target);
    }
    }
}
