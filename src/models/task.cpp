#include "task.h"

QString TaskInfo::getName() const
{
    return name;
}

void TaskInfo::setName(const QString &value)
{
    name = value;
}

qint64 TaskInfo::getPriority() const
{
    return priority;
}

void TaskInfo::setPriority(qint64 value)
{
    priority = value;
}

qint64 TaskInfo::getBurstTime() const
{
    return burstTime;
}

void TaskInfo::setBurstTime(qint64 value)
{
    burstTime = value;
}

bool TaskInfo::depends() const
{
    return !getResource().isEmpty();
}

QString TaskInfo::getResource() const
{
    return resource;
}

void TaskInfo::setResource(const QString &value)
{
    resource = value;
}

Task::Task(Task *parent)
{
    this->parent = parent;
}

Task::~Task()
{
    const auto stamp = getFinishTime();
    for (auto iterator = children.begin(); iterator != children.end(); ++iterator)
    {
        iterator->get()->setFinishTime(stamp);
    }
}

void Task::setState(State value)
{
    state = value;
}

Task::State Task::getState() const
{
    return state;
}

qint64 Task::getIdentifier() const
{
    return identifier;
}

void Task::setIdentifier(qint64 value)
{
    identifier = value;
}

qint64 Task::getRemainingTime() const
{
    return remainingTime;
}

void Task::setRemainingTime(qint64 value)
{
    remainingTime = value;
}

Task *Task::find(qint64 value)
{
    if (value < getMinimumID() && value > getMaximumID()) {
        return nullptr;
    }
    else if(this->identifier == value) {
        return this;
    }
    else {
        for (auto iterator = children.begin(); iterator != children.end(); ++iterator)
        {
            return iterator->get()->find(value);
        }
    }
    return nullptr;
}

qint64 Task::getMinimumID()
{
    return 0;
}

qint64 Task::getMaximumID()
{
    return 10000 - 1;
}

qint64 Task::getStartTime() const
{
    return startTime;
}

void Task::setStartTime(qint64 value)
{
    startTime = value;
}

qint64 Task::getFinishTime() const
{
    return finishTime;
}

void Task::setFinishTime(qint64 value)
{
    finishTime = value;
}

Task* Task::getParent()
{
    return parent;
}

void Task::setParent(Task *value)
{
    parent = value;
}

void Task::addChild(std::unique_ptr<Task> item)
{
    if (!item)
    {
        item->setParent(this);
        children.push_back(std::move(item));
    }
}

void Task::removeChild(int row)
{
    if (row < 0 || row >= children.size())
    {
        return;
    }
    children.erase(children.begin() + row);
}

qint64 Task::childCount() const
{
    return children.size();
}

Task *Task::getChild(int row)
{
    return row >= 0 && row < children.size() ? children.at(row).get() : nullptr;
}

qint64 Task::row() const
{
    if (parent)
    {
        const auto &children = parent->children;
        auto iterator = std::find_if(children.cbegin(), children.cend(),
            [this](const std::unique_ptr<Task> &value)
            {
                if (value.get() == this)
                {
                    return true;
                }
                return false;
            }
        );
        if (iterator == children.cend())
        {
            Q_ASSERT(false);
            return -1;
        }
        return std::distance(children.cbegin(), iterator);
    }
    else
    {
        return 0;
    }
}

qint64 Task::columnCount() const
{
    auto count = 0;

    // Command
    count++;

    // State
    count++;

    // Resources
    count++;

    // Priority
    count++;

    return count;
}
