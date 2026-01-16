#include "readymodel.h"
#include "core/task.h"
#include <QColor>

ReadyModel::ReadyModel(qsizetype maximum, QObject *parent) : PriorityModel(parent)
{
    setMaximumSize(maximum);
}

QVariant ReadyModel::headerData(int section, Qt::Orientation orientation, int role) const
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
    default:
    {
        return {};
    }
    }
}

bool ReadyModel::hasCapacity() const
{
    if (maximum <= 0)
    {
        return true;
    }
    return container.size() < maximum;
}

qsizetype ReadyModel::maximumSize() const
{
    return maximum;
}

bool ReadyModel::insertTask(const QModelIndex &index)
{
    if (!hasCapacity()) {
        return false;
    }
    return PriorityModel::insertTask(index);
}

void ReadyModel::setMaximumSize(qsizetype size)
{
    maximum = size;
    container.reserve(size);
}

int ReadyModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
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

QVariant ReadyModel::data(const QModelIndex &index, int role) const
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
            bool ok = false;
            auto total = target.data(Task::BurstTimeRole).toLongLong(&ok);
            if(!ok) {
                return {};
            }
            auto passed = target.data(Task::QuantumRole).toLongLong(&ok);
            if (!ok) {
                return {};
            }
            return QString("%1%").arg((passed / total) * 100);
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
