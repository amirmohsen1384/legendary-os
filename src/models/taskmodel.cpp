#include "taskmodel.h"
#include <QColor>

Task* TaskModel::createTask(const TaskInfo &info, Task *parent)
{
    if (!parent)
    {
        return {};
    }

    auto task = std::make_unique<Task>(parent);
    task->setBurstTime(info.getBurstTime());
    task->setPriority(info.getPriority());
    task->setAgent(info.getAgent());
    task->setName(info.getName());

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
            case Task::State::Timeout:
            {
                return "Timeout";
            }
            case Task::State::WaitingForLimit:
            case Task::State::WaitingForAgent:
            {
                return "Waiting";
            }
            }
        }
        }
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
            case Task::State::Timeout:
            {
                return QColor(200, 230, 120); // Light Lemon
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
                return QColor(Qt::darkRed);
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
    case Task::NameRole:
    {
        return item->getName();
    }
    case Task::PriorityRole:
    {
        return item->getPriority();
    }
    case Task::BurstTimeRole:
    {
        return item->getBurstTime();
    }
    case Task::AgentRole:
    {
        return item->getAgent();
    }
    case Task::StateRole:
    {
        return QVariant::fromValue(item->getState());
    }
    case Task::PIDRole:
    {
        return item->getIdentifier();
    }
    case Task::QuantumRole:
    {
        return item->getQuantum();
    }
    case Task::StartTimeRole:
    {
        return item->getStartTime();
    }
    case Task::FinishTimeRole:
    {
        return item->getFinishTime();
    }
    case Task::RemainingTimeRole:
    {
        return item->getRemainingTime();
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
    case TaskInfo::NameRole:
    {
        if(value.canConvert<QString>())
        {
            item->setName(value.toString());
            changed = true;
        }
        break;
    }
    case TaskInfo::PriorityRole:
    {
        if (value.canConvert<qint64>())
        {
            item->setPriority(value.toLongLong());
            changed = true;
        }
        break;
    }
    case TaskInfo::BurstTimeRole:
    {
        if (value.canConvert<qint64>())
        {
            item->setBurstTime(value.toLongLong());
            changed = true;
        }
        break;
    }
    case TaskInfo::AgentRole:
    {
        if (value.canConvert<QString>())
        {
            item->setAgent(value.toString());
            changed = true;
        }
        break;
    }
    case TaskInfo::StateRole:
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
    const auto row = index.row();
    const auto item = index.parent();
    auto parent = !item.isValid() ? root.get() : static_cast<Task*>(item.internalPointer());
    beginRemoveRows(item, row, row);
    parent->removeChild(row);
    endRemoveRows();
    return true;
}

void TaskModel::beginToProceed(const QModelIndex &index, qint64 timestamp)
{
    if (!index.isValid())
    {
        return;
    }
    auto item = static_cast<Task*>(index.internalPointer());
    if(item->beginToProceed(timestamp));
    {
        QList<int> roles;
        roles.append(Task::StateRole);
        roles.append(Qt::DisplayRole);
        roles.append(Qt::BackgroundRole);
        roles.append(Qt::ForegroundRole);
        roles.append(Task::StartTimeRole);
        emit dataChanged(index, index, roles);
    }
}

void TaskModel::endToProceed(const QModelIndex &index, qint64 timestamp)
{
    if (!index.isValid())
    {
        return;
    }
    auto item = static_cast<Task*>(index.internalPointer());
    if(item->endToProceed(timestamp))
    {
        QList<int> roles;
        roles.append(Task::StateRole);
        roles.append(Qt::DisplayRole);
        roles.append(Qt::BackgroundRole);
        roles.append(Qt::ForegroundRole);
        roles.append(Task::PriorityRole);
        roles.append(Task::FinishTimeRole);
        emit dataChanged(index, index, roles);
    }
}

qint64 TaskModel::proceed(const QModelIndex &index, qint64 quantum)
{
    if (!index.isValid())
    {
        return 0;
    }
    auto item = static_cast<Task*>(index.internalPointer());
    auto result = item->proceed(quantum);
    if (result > 0)
    {
        QList<int> roles;
        roles.append(Qt::DisplayRole);
        roles.append(Task::QuantumRole);
        roles.append(Qt::BackgroundRole);
        roles.append(Qt::ForegroundRole);
        roles.append(Task::PriorityRole);
        roles.append(Task::FinishTimeRole);
        roles.append(Task::RemainingTimeRole);
        emit dataChanged(index, index, roles);
    }
    return result;
}

void TaskModel::clear()
{
    beginResetModel();
    root.reset(nullptr);
    root = std::make_unique<Task>(nullptr);
    root->setName("Root");
    endResetModel();
}
