#include "processmodel.h"

ProcessModel::ProcessModel(QObject *parent) : QAbstractItemModel(parent)
{}

QVariant ProcessModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
    {
        return {};
    }
    switch(role)
    {
    case Qt::DisplayRole:
    {
        auto group = static_cast<Info>(section);
        switch (group)
        {
        case Info::PID: {
            return QString("PID");
        }
        case Info::Name: {
            return QString("Name");
        }
        case Info::State: {
            return QString("State");
        }
        case Info::Dependency: {
            return QString("Dependency");
        }
        case Info::Priority: {
            return QString("Priority");
        }
        default: {
            return {};
        }
        }
        break;
    }
    case Qt::BackgroundRole: {
        return QColor(220, 220, 220);
    }
    case Qt::TextAlignmentRole: {
        return Qt::AlignCenter;
    }
    default: {
        return {};
    }
    }
}

QModelIndex ProcessModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
    {
        return {};
    }
    auto ancestor = parent.isValid() ? static_cast<ProcessInfo*>(parent.internalPointer()) : root.get();
    auto result = ancestor->subProcess(row);
    return !result ? QModelIndex() : createIndex(row, column, result);
}

QModelIndex ProcessModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return {};
    }
    else
    {
        auto item = static_cast<ProcessInfo*>(index.internalPointer());
        auto parent = item->getParent();
        if (parent == nullptr || parent == root.get())
        {
            return {};
        }
        return createIndex(item->row(), 0, parent);
    }
}

int ProcessModel::rowCount(const QModelIndex &parent) const
{
    auto item = !parent.isValid() ? root.get() : static_cast<ProcessInfo*>(parent.internalPointer());
    return !item ? item->subProcessCount() : 0;
}

int ProcessModel::columnCount(const QModelIndex &parent) const
{
    auto item = parent.isValid() ? static_cast<ProcessInfo*>(parent.internalPointer()) : root.get();
    return !item ? item->columnCount() : 0;
}

QVariant ProcessModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    auto item = static_cast<ProcessInfo*>(index.internalPointer());
    if(!item) {
        return {};
    }

    switch (role) {
    case Qt::DisplayRole:
    {
        auto group = static_cast<Info>(info.column);
        switch (group)
        {
        case Info::Name: {
            return item->getName();
        }
        case Info::Dependency: {
            if (item->needsFile())
            {
                const auto &data = item->getFileName();
                if (data.size() > 16) {
                    return QString("%1...").arg(data.first(16));
                } else {
                    return data;
                }
            }
            else
            {
                return "No Dependency";
            }
        }
        case Info::PID: {
            return item->getIdentifier();
        }
        case Info::Priority: {
            return item->getPriority();
        }
        case Info::State: {
            switch (item->getState()) {
            case ProcessInfo::State::Running: {
                return "Running";
            }
            case ProcessInfo::State::Ready: {
                return "Ready";
            }
            case ProcessInfo::State::WaitingForFile:
            case ProcessInfo::State::WaitingForLimit: {
                return "Waiting";
            }
            }
        }
        default: {
            return {};
        }
    }
    case Qt::TextAlignmentRole: {
        return Qt::AlignCenter;
    }
    case Qt::BackgroundRole: {
        auto group = static_cast<Info>(info.column);
        switch (group) {
        case Info::State: {
            switch (item->getState()) {
            case ProcessInfo::State::Running: {
                return QColor(150, 255, 180); // Light Green
            }
            case ProcessInfo::State::Ready: {
                return QColor(230, 255, 150); // Light Yellow
            }
            case ProcessInfo::State::WaitingForFile:
            case ProcessInfo::State::WaitingForLimit: {
                return QColor(170, 230, 255); // Light Blue
            }
            }
        }
        default: {
            return {};
        }
        }
    }
    case Qt::ForegroundRole: {
        auto group = static_cast<Info>(info.column);
        switch(group) {
        case Info::Dependency: {
            if (!item->needsFile()) {
                return Qt::red;
            }
            else {
                return {};
            }
        }
        default: {
            return {};
        }
        }
    }
    case Qt::UserRole:
    {
        return QVariant::fromValue(item);
    }
    default:
    {
        return {};
    }
    }
    }
}
