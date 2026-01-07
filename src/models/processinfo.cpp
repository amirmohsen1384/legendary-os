#include "processinfo.h"

ProcessInfo::ProcessInfo(ProcessInfo *parent) : parent(parent)
{}

qint64 ProcessInfo::getIdentifier() const
{
    return identifier;
}

void ProcessInfo::setIdentifier(qint64 value)
{
    identifier = value;
}

void ProcessInfo::setCommand(Command value)
{
    command = value;
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

void ProcessInfo::setState(State value)
{
    state = value;
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

qint64 ProcessInfo::getStartTime() const
{
    return startTime;
}

void ProcessInfo::setStartTime(qint64 value)
{
    startTime = value;
}

qint64 ProcessInfo::getFinishTime() const
{
    return finishTime;
}

void ProcessInfo::setFinishTime(qint64 value)
{
    finishTime = value;
}

ProcessInfo *ProcessInfo::getParent() const
{
    return parent;
}

void ProcessInfo::setParent(ProcessInfo *value)
{
    parent = value;
}

void ProcessInfo::addSubprocess(std::unique_ptr<ProcessInfo> process)
{
    children.push_back(std::move(process));
}

qint64 ProcessInfo::subProcessCount() const
{
    return children.size();
}

ProcessInfo *ProcessInfo::subProcess(int row)
{
    return row >= 0 && row < children.size() ? children.at(row).get() : nullptr;
}

qint64 ProcessInfo::row() const
{
    if (parent)
    {
        const auto &children = parent->children;
        auto iterator = std::find_if(children.cbegin(), children.cend(),
            [this](const std::unique_ptr<ProcessInfo> &value)
            {
                if (value.get() == this)
                {
                    return true;
                }
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

qint64 ProcessInfo::columnCount() const
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

ProcessInfo::State ProcessInfo::getState() const
{
    return state;
}

ProcessInfo::Command ProcessInfo::getCommand() const
{
    return command;
}
