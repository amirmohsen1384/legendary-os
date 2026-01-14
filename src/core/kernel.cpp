#include "kernel.h"
#include <QThread>

void CentralKernel::dispatch(const QPersistentModelIndex &index)
{
    if (readyTasks->hasCapacity())
    {
        readyTasks->insertTask(index);
    }
    else
    {
        tasks->setData(index, QVariant::fromValue(Task::State::WaitingForLimit), Task::StateRole);
        waitingTasks->insert(index);
    }
}

CentralKernel::CentralKernel(const Config::Info &info, QObject *parent) : QObject{parent}
{
    locked = false;
    settings = info;
    elapsedQuantum = 0;
    scheduleForShutdown = false;
    inputLimit = settings.inputCommandLimit;
}

void CentralKernel::addTask(const TaskInfo &info, const QModelIndex &parent)
{
    if(!hasPrerequisits())
    {
        return;
    }
    auto index = tasks->insert(info, parent);
    if (!info.depends())
    {
        dispatch(index);
    }
    else
    {
        auto agent = info.getAgent();
        if (!agents->index(agent).isValid())
        {
            tasks->setData(index, QVariant::fromValue(Task::State::WaitingForAgent), Task::StateRole);
            waitingTasks->insert(index);
        }
        else
        {
            dispatch(index);
        }
    }
}

void CentralKernel::removeTask(const QModelIndex &index)
{
    if (!hasPrerequisits())
    {
        return;
    }
    tasks->setData(index, QVariant::fromValue(Task::State::Unknown), Task::StateRole);
    waitingTasks->remove(index);
    readyTasks->remove(index);
    tasks->remove(index);
}

void CentralKernel::addAgent(const AgentInfo &info, const QModelIndex &parent)
{
    if (!hasPrerequisits())
    {
        return;
    }
    agents->insert(info, parent);
}

void CentralKernel::removeAgent(const QModelIndex &index)
{
    if (!hasPrerequisits())
    {
        return;
    }
    agents->remove(index);
}

void CentralKernel::shutdown()
{
    if (!scheduleForShutdown)
    {
        scheduleForShutdown = true;
        emit scheduledForShutdown();
    }
}

void CentralKernel::execute()
{
}

bool CentralKernel::isLocked() const
{
    return locked;
}

TaskModel *CentralKernel::taskModel()
{
    return tasks;
}

AgentModel *CentralKernel::agentModel()
{
    return agents;
}

LoggingModel *CentralKernel::loggingModel()
{
    return logger;
}

ReadyTaskModel *CentralKernel::readyTaskModel()
{
    return readyTasks;
}

WaitingTaskModel *CentralKernel::waitingTaskModel()
{
    return waitingTasks;
}

bool CentralKernel::hasPrerequisits() const
{
    return tasks && waitingTasks && readyTasks && logger && agents;
}

void CentralKernel::lock()
{
    setLocked(true);
}

void CentralKernel::unlock()
{
    setLocked(false);
}

void CentralKernel::setLocked(bool value)
{
    locked = value;
    emit lockChanged(value);
}

void CentralKernel::setTaskModel(TaskModel *model)
{
    tasks = model;
    emit taskModelChanged();
}

void CentralKernel::setAgentModel(AgentModel *model)
{
    agents = model;
    emit agentModelChanged();
}

void CentralKernel::setLoggingModel(LoggingModel *model)
{
    logger = model;
    emit loggingModelChanged();
}

void CentralKernel::setReadyTaskModel(ReadyTaskModel *model)
{
    readyTasks = model;
    emit readyTaskModelChanged();
}

void CentralKernel::setWaitingTaskModel(WaitingTaskModel *model)
{
    waitingTasks = model;
    emit waitingTaskModelChanged();
}
