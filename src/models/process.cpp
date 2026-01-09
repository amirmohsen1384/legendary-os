#include "process.h"

QString ProcessInfo::getName() const
{
    return name;
}

void ProcessInfo::setName(const QString &value)
{
    name = value;
}

qint64 ProcessInfo::getPriority() const
{
    return priority;
}

void ProcessInfo::setPriority(qint64 value)
{
    priority = value;
}

qint64 ProcessInfo::getBurstTime() const
{
    return burstTime;
}

void ProcessInfo::setBurstTime(qint64 value)
{
    burstTime = value;
}

bool ProcessInfo::needsFile() const
{
    return !getFileName().isEmpty();
}

QString ProcessInfo::getFileName() const
{
    return fileName;
}

void ProcessInfo::setFileName(const QString &value)
{
    fileName = value;
}

Process::Process(Process *parent) : parent(parent)
{}

Process::~Process()
{
    const auto stamp = getFinishTime();
    for (auto iterator = children.begin(); iterator != children.end(); ++iterator)
    {
        iterator->get()->setFinishTime(stamp);
    }
}

void Process::setState(State value)
{
    state = value;
}

Process::State Process::getState() const
{
    return state;
}

qint64 Process::getIdentifier() const
{
    return identifier;
}

void Process::setIdentifier(qint64 value)
{
    identifier = value;
}

qint64 Process::getRemainingTime() const
{
    return remainingTime;
}

void Process::setRemainingTime(qint64 value)
{
    remainingTime = value;
}

qint64 Process::getStartTime() const
{
    return startTime;
}

void Process::setStartTime(qint64 value)
{
    startTime = value;
}

qint64 Process::getFinishTime() const
{
    return finishTime;
}

void Process::setFinishTime(qint64 value)
{
    finishTime = value;
}

Process* Process::getParent() const
{
    return parent;
}

void Process::setParent(Process *value)
{
    parent = value;
}

void Process::addChild(std::unique_ptr<Process> process)
{
    children.push_back(std::move(process));
}

void Process::removeChild(int row)
{
    if (row < 0 || row >= children.size()) {
        return;
    }
    children.erase(children.begin() + row);
}

qint64 Process::childCount() const
{
    return children.size();
}

Process *Process::getChild(int row)
{
    return row >= 0 && row < children.size() ? children.at(row).get() : nullptr;
}

qint64 Process::row() const
{
    if (parent)
    {
        const auto &children = parent->children;
        auto iterator = std::find_if(children.cbegin(), children.cend(),
            [this](const std::unique_ptr<Process> &value)
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

qint64 Process::columnCount() const
{
    auto count = 0;

    // Command
    count++;

    // State
    count++;

    // Needs File
    count++;

    // Identifier
    count++;

    // Priority
    count++;

    return count;
}
