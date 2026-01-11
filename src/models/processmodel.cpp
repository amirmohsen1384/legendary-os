#include <QRandomGenerator64>
#include "processmodel.h"
#include <QColor>

Process* ProcessModel::createProcess(const ProcessInfo &info, Process *parent)
{
    if (!parent)
    {
        return {};
    }

    auto process = std::make_unique<Process>(parent);
    process->setName(info.getName());
    process->setPriority(info.getPriority());
    process->setFileName(info.getFileName());
    process->setBurstTime(info.getBurstTime());
    process->setRemainingTime(info.getBurstTime());

    auto pid = QRandomGenerator64::global()->bounded(Process::getMinimumID(), Process::getMaximumID());
    process->setIdentifier(pid);

    auto result = process.get();
    parent->addChild(std::move(process));

    return result;
}

ProcessModel::ProcessModel(QObject *parent) : QAbstractItemModel(parent)
{
    root = std::make_unique<Process>();
    root->setName("Root Node");
}

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
        case Info::PID:
        {
            return QString("PID");
        }
        case Info::Name:
        {
            return QString("Name");
        }
        case Info::State:
        {
            return QString("State");
        }
        case Info::Dependency:
        {
            return QString("Dependency");
        }
        case Info::Priority:
        {
            return QString("Priority");
        }
        default:
        {
            return {};
        }
        }
        break;
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

QModelIndex ProcessModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
    {
        return {};
    }
    auto ancestor = parent.isValid() ? static_cast<Process*>(parent.internalPointer()) : root.get();
    if(!ancestor)
    {
        ancestor = root.get();
    }
    auto result = ancestor->getChild(row);
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
        auto item = static_cast<Process*>(index.internalPointer());
        if (!item)
        {
            item = root.get();
        }
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
    auto item = !parent.isValid() ? root.get() : static_cast<Process*>(parent.internalPointer());
    return item ? item->childCount() : 0;
}

int ProcessModel::columnCount(const QModelIndex &parent) const
{
    auto item = parent.isValid() ? static_cast<Process*>(parent.internalPointer()) : root.get();
    return item ? item->columnCount() : 0;
}

QVariant ProcessModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return {};
    }

    auto item = static_cast<Process*>(index.internalPointer());
    if(!item)
    {
        return {};
    }

    switch (role)
    {
    case Qt::DisplayRole:
    {
        auto group = static_cast<Info>(index.column());
        switch (group)
        {
        case Info::Name:
        {
            return item->getName();
        }
        case Info::Dependency:
        {
            if (item->needsFile())
            {
                const auto &fileName = item->getFileName();
                return fileName.size() <= 16 ? fileName : QString("%1...").arg(fileName.first(16));
            }
            else
            {
                return QString("No Dependency");
            }
        }
        case Info::PID:
        {
            return item->getIdentifier();
        }
        case Info::Priority:
        {
            auto priority = item->getPriority();
            if (priority < 20)
            {
                return "Low";
            }
            else if (priority >= 20 && priority < 50)
            {
                return "Medium";
            }
            else if (priority >= 50 && priority < 80)
            {
                return "Fairly High";
            }
            else if (priority >= 80)
            {
                return "High";
            }
        }
        case Info::State:
        {
            switch (item->getState())
            {
            case Process::State::Unknown:
            {
                return "Unknown";
            }
            case Process::State::Running:
            {
                return "Running";
            }
            case Process::State::Ready:
            {
                return "Ready";
            }
            case Process::State::WaitingForFile:
            case Process::State::WaitingForLimit:
            {
                return "Waiting";
            }
            }
        }
        default:
        {
            return {};
        }
        }
        break;
    }
    case Qt::TextAlignmentRole:
    {
        return Qt::AlignCenter;
    }
    case Qt::BackgroundRole:
    {
        auto group = static_cast<Info>(index.column());
        switch (group)
        {
        case Info::State:
        {
            switch (item->getState())
            {
            case Process::State::Running:
            {
                return QColor(150, 255, 180); // Light Green
            }
            case Process::State::Ready:
            {
                return QColor(230, 255, 150); // Light Yellow
            }
            case Process::State::WaitingForFile:
            case Process::State::WaitingForLimit:
            {
                return QColor(170, 230, 255); // Light Blue
            }
            }
        }
        default:
        {
            return {};
        }
        }
    }
    case Qt::ForegroundRole:
    {
        auto group = static_cast<Info>(index.column());
        switch(group)
        {
        case Info::Dependency:
        {
            if (!item->needsFile())
            {
                return QColor(Qt::red);
            }
            else
            {
                return {};
            }
        }
        case Info::Priority:
        {
            auto priority = item->getPriority();
            if (priority < 20)
            {
                return QColor(200, 0, 0);
            }
            else if (priority >= 20 && priority < 50)
            {
                return QColor(200, 200, 40);
            }
            else if (priority >= 50 && priority < 80)
            {
                return QColor(90, 200, 90);
            }
            else if (priority >= 80)
            {
                return QColor(90, 200, 200);
            }
        }
        default:
        {
            return {};
        }
        }
    }
    case Qt::ToolTipRole:
    {
        auto group = static_cast<Info>(index.column());
        switch(group)
        {
        case Info::Dependency:
        {
            return item->getFileName();
        }
        case Info::Priority:
        {
            return QString("Priority: %1%").arg(item->getPriority());
        }
        default:
        {
            return {};
        }
        }
    }
    case ProcessInfo::Name:
    {
        return item->getName();
    }
    case ProcessInfo::Priority:
    {
        return item->getPriority();
    }
    case ProcessInfo::BurstTime:
    {
        return item->getBurstTime();
    }
    case ProcessInfo::FileName:
    {
        return item->getFileName();
    }
    case ProcessInfo::State:
    {
        return QVariant::fromValue(item->getState());
    }
    default:
    {
        return {};
    }
    }
}

bool ProcessModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool changed = false;
    if (!index.isValid())
    {
        return changed;
    }
    auto item = static_cast<Process*>(index.internalPointer());
    switch (role)
    {
    case ProcessInfo::Name:
    {
        if(value.canConvert<QString>())
        {
            item->setName(value.toString());
            changed = true;
        }
        break;
    }
    case ProcessInfo::Priority:
    {
        if (value.canConvert<qint64>())
        {
            item->setPriority(value.toLongLong());
            changed = true;
        }
        break;
    }
    case ProcessInfo::BurstTime:
    {
        if (value.canConvert<qint64>())
        {
            item->setBurstTime(value.toLongLong());
            changed = true;
        }
        break;
    }
    case ProcessInfo::FileName:
    {
        if (value.canConvert<QString>())
        {
            item->setFileName(value.toString());
            changed = true;
        }
        break;
    }
    case ProcessInfo::State:
    {
        if (value.canConvert<Process::State>())
        {
            item->setState(qvariant_cast<Process::State>(value));
            changed = true;
        }
        break;
    }
    }
    if (changed)
    {
        emit dataChanged(index, index, {role, Qt::DisplayRole, Qt::BackgroundRole, Qt::ForegroundRole});
    }
    return changed;
}

QModelIndex ProcessModel::index(qint64 value, const QModelIndex &parent)
{
    auto ancestor = !parent.isValid() ? root.get() : static_cast<Process*>(parent.internalPointer());
    if (!ancestor)
    {
        return {};
    }
    auto item = ancestor->find(value);
    return !item ? QModelIndex() : createIndex(item->row(), 0, item);
}

bool ProcessModel::insert(const ProcessInfo &info, const QModelIndex &parent)
{
    auto ancestor = !parent.isValid() ? root.get() : static_cast<Process*>(parent.internalPointer());
    if(!ancestor)
    {
        return false;
    }
    beginInsertRows(parent, ancestor->childCount(), ancestor->childCount());
    auto process = createProcess(info, ancestor);
    endInsertRows();
    return process != nullptr;
}

bool ProcessModel::remove(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return false;
    }
    const auto item = index.parent();
    auto parent = !item.isValid() ? root.get() : static_cast<Process*>(item.internalPointer());
    beginRemoveRows(item, index.row(), index.row()); // This causes some problems.
    parent->removeChild(index.row());
    endRemoveRows();
    return true;
}

void ProcessModel::clear()
{
    beginResetModel();
    root.reset(nullptr);
    root = std::make_unique<Process>(nullptr);
    endResetModel();
}
