#include "task.h"
#include <QRandomGenerator64>

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

bool TaskInfo::isValid() const
{
    return !name.isEmpty();
}

bool TaskInfo::depends() const
{
    return !getAgent().isEmpty();
}

QString TaskInfo::getAgent() const
{
    return agent;
}

void TaskInfo::setAgent(const QString &value)
{
    agent = value;
}

Task::Task(Task *parent)
{
    this->parent = parent;
    auto pid = QRandomGenerator64::global()->bounded(1, 10000 - 1);
    identifier = pid;
}

Task::~Task()
{
    const auto stamp = getFinishTime();
    for (auto iterator = children.begin(); iterator != children.end(); ++iterator)
    {
        iterator->get()->finishTime = stamp;
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

qint64 Task::getRemainingTime() const
{
    return getBurstTime() - getQuantum();
}

void Task::setBurstTime(qint64 value)
{
    TaskInfo::setBurstTime(value);
}

bool Task::beginToProceed(qint64 timestamp)
{
    if (finished() || state == State::Running)
    {
        return false;
    }
    if (startTime < 0)
    {
        startTime = timestamp;
    }
    state = State::Running;
    return true;
}

bool Task::endToProceed(qint64 timestamp)
{
    if (state != State::Running)
    {
        return false;
    }
    else if (finished())
    {
        finishTime = timestamp;
        state = State::Ready;
    }
    else
    {
        state = State::Timeout;
    }
    return true;
}

qint64 Task::proceed(qint64 value)
{
    if (state != State::Running || finished())
    {
        return 0;
    }
    qint64 rest = getRemainingTime();
    qint64 spent = rest >= value ? value : rest;
    quantum += spent;
    return spent;
}

bool Task::finished() const
{
    return getRemainingTime() == 0;
}

qint64 Task::getStartTime() const
{
    return startTime;
}

qint64 Task::getQuantum() const
{
    return quantum;
}

qint64 Task::getFinishTime() const
{
    return finishTime;
}

Task* Task::getParent()
{
    return parent;
}

QString Task::text(const State &state)
{
    switch (state)
    {
    case Task::State::WaitingForLimit:
    {
        return QStringLiteral("Waiting For Limit");
    }
    case Task::State::WaitingForAgent:
    {
        return QStringLiteral("Waiting For Agent");
    }
    case Task::State::Terminate:
    {
        return QStringLiteral("Terminate");
    }
    case Task::State::Execute:
    {
        return QStringLiteral("Execute");
    }
    case Task::State::Running:
    {
        return QStringLiteral("Running");
    }
    case Task::State::Unknown:
    {
        return QStringLiteral("Unknown");
    }
    case Task::State::Timeout:
    {
        return QStringLiteral("Timeout");
    }
    case Task::State::Error:
    {
        return QStringLiteral("Error");
    }
    case Task::State::Ready:
    {
        return QStringLiteral("Ready");
    }
    }
    return QStringLiteral("Invalid State");
}

void Task::setParent(Task *value)
{
    parent = value;
}

void Task::addChild(std::unique_ptr<Task> item)
{
    if (item)
    {
        item->setParent(this);
        children.push_back(std::move(item));
    }
}

bool Task::removeChild(int row)
{
    if (row < 0 || row >= qsizetype(children.size()))
    {
        return false;
    }
    children.erase(children.cbegin() + row);
    return true;
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

    // Agent
    count++;

    // Priority
    count++;

    return count;
}
