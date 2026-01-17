#include "coordinator.h"
#include <QMutexLocker>

Coordinator::Coordinator(const Config::Info &info, QObject *parent) : QThread(parent), settings(info)
{}

qint64 Coordinator::getElapsedQuantums()
{
    QMutexLocker locker(&mutex);
    return elapsedQuantums;
}

qint64 Coordinator::getUnusedQuantums()
{
    QMutexLocker locker(&mutex);
    return unusedQuantums;
}

TaskModel *Coordinator::getTasks()
{
    return tasks;
}

AgentModel *Coordinator::getAgents()
{
    return agents;
}

LoggingModel *Coordinator::getLogs()
{
    return logs;
}

ReadyModel *Coordinator::getReadyTasks()
{
    return readyTasks;
}

PriorityModel *Coordinator::getLimitTasks()
{
    return limitTasks;
}

PriorityModel *Coordinator::getAgentTasks()
{
    return agentTasks;
}

bool Coordinator::insertAgent(const AgentInfo &info, const QModelIndex &parent)
{
    auto index = agents->insertAgent(info, parent);
    if (!index.isValid())
    {
        return false;
    }
    const auto currentPath = agents->toString(index);
    for (auto i = 0; i < agentTasks->rowCount(); ++i)
    {
        auto target = agentTasks->toTask(agentTasks->index(i, 0));
        auto targetPath = target.data(Task::AgentRole).toString();
        if (targetPath == currentPath)
        {
            agentTasks->removeRow(i--);
            if (readyTasks->hasCapacity())
            {
                auto result = tasks->setState(target, Task::State::Ready);
                if (result.successful)
                {
                    readyTasks->insertTask(target);
                    logTask(target, result.previous, result.current);
                }
            }
            else
            {
                auto result = tasks->setState(target, Task::State::WaitingForLimit);
                if (result.successful)
                {
                    limitTasks->insertTask(target);
                    logTask(target, result.previous, result.current);
                }
            }
        }
    }
    return true;
}

bool Coordinator::insertTask(const TaskInfo &info, const QModelIndex &parent)
{
    if (!hasReqiurements()) {
        return false;
    }
    auto index = tasks->insertTask(info, parent);
    if (!index.isValid()) {
        return false;
    }
    dispatch(index);
    return true;
}

bool Coordinator::removeAgent(const QModelIndex &agent)
{
    for (auto i = 0; i < agents->rowCount(agent); ++i)
    {
        auto child = agents->index(i, 0, agent);
        if (!removeAgent(child))
        {
            return false;
        }
    }
    auto path = agents->toString(agent);
    for (auto i = 0; i < readyTasks->rowCount(); ++i)
    {
        auto index = readyTasks->toTask(readyTasks->index(i, 0));
        auto current = agent.data(Task::AgentRole).toString();
        if(current.isEmpty())
        {
            continue;
        }
        else if (path == current)
        {
            readyTasks->removeRow(i--);
            agentTasks->insertTask(index);
        }
    }
    for (auto i = 0; i < limitTasks->rowCount(); ++i)
    {
        auto index = limitTasks->toTask(limitTasks->index(i, 0));
        auto current = agent.data(Task::AgentRole).toString();
        if(current.isEmpty())
        {
            continue;
        }
        else if (path == current)
        {
            limitTasks->removeRow(i--);
            agentTasks->insertTask(index);
        }
    }
    return agents->removeAgent(agent);
}

bool Coordinator::removeTask(const QModelIndex &task)
{
    for (auto i = 0; i < tasks->rowCount(task); ++i)
    {
        auto child = tasks->index(i, 0, task);
        if (!removeTask(child))
        {
            return false;
        }
    }

    for (auto i = 0; i < readyTasks->rowCount(); ++i)
    {
        auto current = readyTasks->toTask(readyTasks->index(i, 0));
        if (task == current)
        {
            readyTasks->removeRow(i--);
            return tasks->removeTask(task);
        }
    }

    for (auto i = 0; i < limitTasks->rowCount(); ++i)
    {
        auto current = limitTasks->toTask(limitTasks->index(i, 0));
        if (task == current)
        {
            limitTasks->removeRow(i--);
            return tasks->removeTask(task);
        }
    }

    for (auto i = 0; i < agentTasks->rowCount(); ++i)
    {
        auto current = agentTasks->toTask(agentTasks->index(i, 0));
        if (task == current)
        {
            agentTasks->removeRow(i--);
            return tasks->removeTask(task);
        }
    }

    return tasks->removeTask(task);
}

void Coordinator::scheduleShutdown()
{
    if (!shudownSchedule)
    {
        QMutexLocker locker(&mutex);
        shudownSchedule = true;
        emit shutdownScheduled();
    }
}

void Coordinator::setTasks(TaskModel *model)
{
    tasks = model;
}

void Coordinator::setAgents(AgentModel *model)
{
    agents = model;
}

void Coordinator::setLogs(LoggingModel *model)
{
    logs = model;
}

void Coordinator::setReadyTasks(ReadyModel *model)
{
    readyTasks = model;
}

void Coordinator::setLimitTasks(PriorityModel *model)
{
    limitTasks = model;
}

void Coordinator::setAgentTasks(PriorityModel *model)
{
    agentTasks = model;
}

void Coordinator::dispatch(const QModelIndex &task)
{
    auto executable = task.data(Task::ExecutableRole).toBool();
    if (executable)
    {
        if (readyTasks->hasCapacity())
        {
            tasks->setState(task, Task::State::Ready);
            readyTasks->insertTask(task);
        }
        else
        {
            tasks->setState(task, Task::State::WaitingForLimit);
            limitTasks->insertTask(task);
        }
    }
    else
    {
        auto path = task.data(Task::AgentRole).toString();
        if (agents->index(path).isValid())
        {
            if (readyTasks->hasCapacity())
            {
                tasks->setState(task, Task::State::Ready);
                readyTasks->insertTask(task);
            }
            else
            {
                tasks->setState(task, Task::State::WaitingForLimit);
                limitTasks->insertTask(task);
            }
        }
        else
        {
            tasks->setState(task, Task::State::WaitingForAgent);
            agentTasks->insertTask(task);
        }
    }
}

bool Coordinator::logTask(const QModelIndex &task, const Task::State &previous, const Task::State &current, const QString &description)
{
    if (!task.isValid()) {
        return false;
    }
    LogInfo information;
    information.name = task.data(Task::NameRole).toString();

    bool ok = false;
    information.identifier = task.data(Task::PIDRole).toLongLong(&ok);
    if (!ok) {
        information.identifier = 0;
    }

    information.timestamp = getElapsedQuantums();
    information.description = description;

    QString path = task.data(Task::AgentRole).toString();
    auto agent = agents->index(path);
    if (agent.isValid())
    {
        information.agent.setName(agent.data(Agent::NameRole).toString());
        information.agent.setDescription(agent.data(Agent::DescriptionRole).toString());
    }
    information.previous = previous;
    information.current = current;

    return logs->log(information);
}



bool Coordinator::hasReqiurements() const
{
    return agents && tasks && logs && limitTasks && agentTasks && tasks->rowCount() > 0;
}

void Coordinator::setElapsedQuantums(qint64 value)
{
    QMutexLocker locker(&mutex);
    elapsedQuantums = value;
    emit quantumElapsed(value);
}

void Coordinator::run()
{
    if (!hasReqiurements())
    {
        return;
    }
    const auto unit = settings.quantumSize;
    for (auto i = 0; i < settings.executionCycle; ++i)
    {
        auto timestamp = getElapsedQuantums();
        if (readyTasks->rowCount() <= 0)
        {
            QMutexLocker locker(&mutex);
            unusedQuantums += unit;
            emit quantumUnused(unusedQuantums);
            continue;
        }

        auto task = readyTasks->peekBest();

        {
            auto before = tasks->getState(task);
            tasks->beginToProceed(task, timestamp);
            logTask(task, before, tasks->getState(task));
        }

        QThread::sleep(settings.pause);

        {
            auto before = tasks->getState(task);
            tasks->proceed(task, unit);
            logTask(task, before, tasks->getState(task));
        }

        QThread::sleep(settings.pause);

        timestamp += unit;
        setElapsedQuantums(timestamp);

        {
            auto before = tasks->getState(task);
            tasks->endToProceed(task, timestamp);
            logTask(task, before, tasks->getState(task));
        }

        QThread::sleep(settings.pause);

        auto state = tasks->getState(task);
        if (state == Task::State::Timeout)
        {
            readyTasks->removeBest();

            bool ok = false;
            auto usedQuantums = tasks->data(task, Task::QuantumRole).toLongLong(&ok);
            auto currentPriority = tasks->data(task, Task::PriorityRole).toLongLong(&ok);
            if (!ok)
            {
                usedQuantums = 0;
                currentPriority = 1;
            }

            auto priority = qint64(currentPriority / (0.5 * usedQuantums + 1));
            tasks->setData(task, priority, Task::PriorityRole);
            {
                auto result = tasks->setState(task, Task::State::Ready);
                if (result.successful)
                {
                    readyTasks->insertTask(task);
                    logTask(task, result.previous, result.current);
                }
            }
        }
        else
        {
            bool ok = false;
            auto time = task.data(Task::RemainingTimeRole).toLongLong(&ok);
            if (!ok)
            {
                time = 0;
            }
            if (time == 0)
            {
                readyTasks->removeBest();
                logTask(task, tasks->getState(task), Task::State::Terminate);

                if(!tasks->removeTask(task))
                {
                    qDebug() << "Failed to remove the task.";
                }
                else
                {
                    while (limitTasks->rowCount() > 0 && readyTasks->hasCapacity())
                    {
                        auto newTask = limitTasks->peekBest();
                        {
                            auto result = tasks->setState(newTask, Task::State::Ready);
                            if (result.successful)
                            {
                                readyTasks->insertTask(newTask);
                                limitTasks->removeBest();
                                logTask(newTask, result.previous, result.current);
                            }
                        }
                    }
                }
            }
            else
            {
                logTask(task, tasks->getState(task), Task::State::Execute);
            }
        }
    }

    QMutexLocker locker(&mutex);
    if (shudownSchedule)
    {
        tasks->removeTask(QModelIndex());
        readyTasks->clear();
        limitTasks->clear();
        agentTasks->clear();
    }
}
