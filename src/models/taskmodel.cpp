#include "taskmodel.h"
#include <QColor>

Task* TaskModel::createTask(const TaskInfo &info, Task *parent)
{
    if (!parent)
    {
        return {};
    }

    auto task = std::make_unique<Task>(parent);
    task->setName(info.getName());
    task->setPriority(info.getPriority());
    task->setAgent(info.getAgent());
    task->setBurstTime(info.getBurstTime());
    task->setRemainingTime(info.getBurstTime());

    auto result = task.get();
    parent->addChild(std::move(task));

    return result;
}

TaskModel::TaskModel(QObject *parent) : QAbstractItemModel(parent)
{
    root = std::make_unique<Task>();
    root->setName("Root");
}

QVariant TaskModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
    {
        return {};
    }
    switch(role)
    {
    case Qt::DisplayRole:
    {
        auto group = static_cast<Header>(section);
        switch (group)
        {
        case Header::Name:
        {
            return QString("Name");
        }
        case Header::State:
        {
            return QString("State");
        }
        case Header::Agent:
        {
            return QString("Agent");
        }
        case Header::Priority:
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

QModelIndex TaskModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
    {
        return {};
    }
    auto ancestor = parent.isValid() ? static_cast<Task*>(parent.internalPointer()) : root.get();
    if(!ancestor)
    {
        ancestor = root.get();
    }
    auto result = ancestor->getChild(row);
    return !result ? QModelIndex() : createIndex(row, column, result);
}

QModelIndex TaskModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return {};
    }
    else
    {
        auto item = static_cast<Task*>(index.internalPointer());
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

int TaskModel::rowCount(const QModelIndex &parent) const
{
    auto item = !parent.isValid() ? root.get() : static_cast<Task*>(parent.internalPointer());
    return item ? item->childCount() : 0;
}

int TaskModel::columnCount(const QModelIndex &parent) const
{
    auto item = parent.isValid() ? static_cast<Task*>(parent.internalPointer()) : root.get();
    return item ? item->columnCount() : 0;
}

QVariant TaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return {};
    }

    auto item = static_cast<Task*>(index.internalPointer());
    if(!item)
    {
        return {};
    }

    switch (role)
    {
    case Qt::DisplayRole:
    {
        auto group = static_cast<Header>(index.column());
        switch (group)
        {
        case Header::Name:
        {
            return item->getName();
        }
        case Header::Agent:
        {
            if (item->depends())
            {
                const auto &agent = item->getAgent();
                return agent.size() <= 16 ? agent : QString("%1...").arg(agent.first(16));
            }
            else
            {
                return QString("Independent");
            }
        }
        case Header::Priority:
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
        case Header::State:
        {
            switch (item->getState())
            {
            case Task::State::Unknown:
            {
                return "Unknown";
            }
            case Task::State::Running:
            {
                return "Running";
            }
            case Task::State::Ready:
            {
                return "Ready";
            }
            case Task::State::WaitingForLimit:
            case Task::State::WaitingForAgent:
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
        auto group = static_cast<Header>(index.column());
        switch (group)
        {
        case Header::State:
        {
            switch (item->getState())
            {
            case Task::State::Running:
            {
                return QColor(150, 255, 180); // Light Green
            }
            case Task::State::Ready:
            {
                return QColor(230, 255, 150); // Light Yellow
            }
            case Task::State::WaitingForLimit:
            case Task::State::WaitingForAgent:
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
        auto group = static_cast<Header>(index.column());
        switch(group)
        {
        case Header::Agent:
        {
            if (!item->depends())
            {
                return QColor(Qt::red);
            }
            else
            {
                return {};
            }
        }
        case Header::Priority:
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
        auto group = static_cast<Header>(index.column());
        switch(group)
        {
        case Header::Agent:
        {
            return item->getAgent();
        }
        case Header::Priority:
        {
            return QString("Priority: %1%").arg(item->getPriority());
        }
        default:
        {
            return {};
        }
        }
    }
    case TaskInfo::Name:
    {
        return item->getName();
    }
    case TaskInfo::Priority:
    {
        return item->getPriority();
    }
    case TaskInfo::BurstTime:
    {
        return item->getBurstTime();
    }
    case TaskInfo::Agent:
    {
        return item->getAgent();
    }
    case TaskInfo::State:
    {
        return QVariant::fromValue(item->getState());
    }
    case TaskInfo::PID:
    {
        return item->getIdentifier();
    }
    default:
    {
        return {};
    }
    }
}

bool TaskModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool changed = false;
    if (!index.isValid())
    {
        return changed;
    }
    auto item = static_cast<Task*>(index.internalPointer());
    switch (role)
    {
    case TaskInfo::Name:
    {
        if(value.canConvert<QString>())
        {
            item->setName(value.toString());
            changed = true;
        }
        break;
    }
    case TaskInfo::Priority:
    {
        if (value.canConvert<qint64>())
        {
            item->setPriority(value.toLongLong());
            changed = true;
        }
        break;
    }
    case TaskInfo::BurstTime:
    {
        if (value.canConvert<qint64>())
        {
            item->setBurstTime(value.toLongLong());
            changed = true;
        }
        break;
    }
    case TaskInfo::Agent:
    {
        if (value.canConvert<QString>())
        {
            item->setAgent(value.toString());
            changed = true;
        }
        break;
    }
    case TaskInfo::State:
    {
        if (value.canConvert<Task::State>())
        {
            item->setState(qvariant_cast<Task::State>(value));
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

QModelIndex TaskModel::index(qint64 value, const QModelIndex &parent)
{
    auto ancestor = !parent.isValid() ? root.get() : static_cast<Task*>(parent.internalPointer());
    if (!ancestor)
    {
        return {};
    }
    auto item = ancestor->find(value);
    return !item ? QModelIndex() : createIndex(item->row(), 0, item);
}

bool TaskModel::insert(const TaskInfo &info, const QModelIndex &parent)
{
    auto ancestor = !parent.isValid() ? root.get() : static_cast<Task*>(parent.internalPointer());
    if(!ancestor)
    {
        return false;
    }
    beginInsertRows(parent, ancestor->childCount(), ancestor->childCount());
    auto task = createTask(info, ancestor);
    endInsertRows();
    return task != nullptr;
}

bool TaskModel::remove(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return false;
    }
    const auto item = index.parent();
    auto parent = !item.isValid() ? root.get() : static_cast<Task*>(item.internalPointer());
    beginRemoveRows(item, index.row(), index.row()); // This causes some problems.
    parent->removeChild(index.row());
    endRemoveRows();
    return true;
}

void TaskModel::clear()
{
    beginResetModel();
    root.reset(nullptr);
    root = std::make_unique<Task>(nullptr);
    root->setName("Root");
    endResetModel();
}
