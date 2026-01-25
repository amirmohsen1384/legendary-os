#include "coordinator.h"
#include <QMutexLocker>

Coordinator::Coordinator(const Settings::Info &info, QObject *parent) : QThread(parent), settings(info)
{
    connect(this, &QThread::started, this, [&]() {
        setState(Coordinator::RunningState);
    });
    connect(this, &QThread::finished, this, [&]() {
        setState(Coordinator::StoppedState);
    });
}

Coordinator::~Coordinator()
{
    if (isRunning())
    {
        setState(Coordinator::StoppedState);
        wait();
    }
}

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

qreal Coordinator::getUtilizationRate()
{
    const auto elapsed = getElapsedQuantums();
    return elapsed > 0 ? 1 - getUnusedQuantums() / elapsed : 0;
}

Coordinator::State Coordinator::getState()
{
    QMutexLocker locker(&mutex);
    return state;
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
    if (isRunning() || !hasReqiurements())
    {
        return false;
    }

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
    if (isRunning() || !hasReqiurements())
    {
        return false;
    }
    auto index = tasks->insertTask(info, parent);
    if (!index.isValid())
    {
        return false;
    }
    dispatch(index);
    return true;
}

bool Coordinator::removeAgent(const QModelIndex &agent)
{
    if (isRunning() || !hasReqiurements())
    {
        return false;
    }
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
    if (isRunning() || !hasReqiurements())
    {
        return false;
    }
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
    if (isRunning() || !hasReqiurements())
    {
        return;
    }
    if (!shudownSchedule)
    {
        QMutexLocker locker(&mutex);
        shudownSchedule = true;
        emit shutdownScheduled();
    }
}

void Coordinator::resume()
{
    if (getState() == State::PausedState)
    {
        setState(State::RunningState);
    }
}

void Coordinator::abort()
{
    setState(State::StoppedState);
}

void Coordinator::pause()
{
    if (getState() == State::RunningState)
    {
        setState(State::PausedState);
    }
}

void Coordinator::setTasks(TaskModel *model)
{
    if (isRunning())
    {
        return;
    }
    tasks = model;
}

void Coordinator::setAgents(AgentModel *model)
{
    if (isRunning())
    {
        return;
    }
    agents = model;
}

void Coordinator::setLogs(LoggingModel *model)
{
    if (isRunning())
    {
        return;
    }
    logs = model;
}

void Coordinator::setReadyTasks(ReadyModel *model)
{
    if (isRunning())
    {
        return;
    }
    readyTasks = model;
}

void Coordinator::setLimitTasks(PriorityModel *model)
{
    if (isRunning())
    {
        return;
    }
    limitTasks = model;
}

void Coordinator::setAgentTasks(PriorityModel *model)
{
    if (isRunning())
    {
        return;
    }
    agentTasks = model;
}

void Coordinator::setState(const State value)
{
    if (getState() == value)
    {
        return;
    }

    QMutexLocker locker(&mutex);
    if (state == StoppedState)
    {
        if (value == PausedState)
        {
            return;
        }
        else if (value == RunningState)
        {
            state = value;
            start(LowPriority);
        }
    }
    else
    {
        state = value;
        if (state != State::PausedState)
        {
            condition.wakeOne();
        }
        emit runningStateChanged(state);
    }
}

void Coordinator::dispatch(const QModelIndex &task)
{
    if (isRunning() || !hasReqiurements())
    {
        return;
    }
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
    if (!hasReqiurements() || !task.isValid())
    {
        qDebug() << "Failed to log the status";
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

    auto result = logs->log(information);
    qDebug() << "Logging Completed. Status:" << result;

    return result;
}



bool Coordinator::hasReqiurements() const
{
    return agents && tasks && logs && limitTasks && agentTasks;
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
        qDebug() << "The kernel does not meet the requirements.";
        return;
    }
    qInfo() << "Running the kernel";

    const auto unit = settings.quantumSize;

    qInfo() << "The quantum size is" << unit;

    for (auto i = 0; i < settings.executionCycle; ++i)
    {
        {
            auto state = getState();
            if (state == State::PausedState)
            {
                qInfo() << "Pausing the kernel...";
                QMutexLocker locker(&mutex);
                condition.wait(&mutex);
            }
            else if (state == State::StoppedState)
            {
                qInfo() << "Forcing the kernel to stop...";
                return;
            }
        }

        auto timestamp = getElapsedQuantums();
        qInfo() << "Current timestamp is" << timestamp;

        if (readyTasks->rowCount() <= 0)
        {
            qWarning() << "No ready task is available to run. Forwading the current quantum.";
            QMutexLocker locker(&mutex);
            unusedQuantums += unit;
            emit quantumUnused(unusedQuantums);
            continue;
        }

        auto task = readyTasks->peekBest();
        qInfo() << "Found one ready task:" << task.data(Task::NameRole).toString();
        {
            auto before = tasks->getState(task);
            qInfo() << "The current state at the beginning is" << qint64(before) << ": Kernel is starting to process.";

            tasks->beginToProceed(task, timestamp);

            qInfo() << "Logging the status...";
            logTask(task, before, tasks->getState(task));
        }

        QThread::sleep(settings.pause);
        {
            auto state = getState();
            if (state == State::PausedState)
            {
                qInfo() << "Pausing the kernel...";
                QMutexLocker locker(&mutex);
                condition.wait(&mutex);
            }
            if (state == State::StoppedState)
            {
                qInfo() << "Forcing the kernel to stop...";
                return;
            }
        }

        {
            auto before = tasks->getState(task);
            qInfo() << "The current state right before processing is" << qint64(before) << ": Kernel is processing.";

            tasks->proceed(task, unit);

            qInfo() << "Logging the status...";
            logTask(task, before, tasks->getState(task));
        }

        QThread::sleep(settings.pause);
        {
            auto state = getState();
            if (state == State::PausedState)
            {
                qInfo() << "Pausing the kernel...";
                QMutexLocker locker(&mutex);
                condition.wait(&mutex);
            }
            if (state == State::StoppedState)
            {
                qInfo() << "Forcing the kernel to stop...";
                return;
            }
        }

        timestamp += unit;
        setElapsedQuantums(timestamp);
        qInfo() << "Current timestamp is" << timestamp;

        {
            auto before = tasks->getState(task);
            qInfo() << "The current state right before processing is" << qint64(before) << ": Kernel is processing.";

            tasks->endToProceed(task, timestamp);

            qInfo() << "Logging the status...";
            logTask(task, before, tasks->getState(task));
        }

        QThread::sleep(settings.pause);
        {
            auto state = getState();
            if (state == State::PausedState)
            {
                qInfo() << "Pausing the kernel...";
                QMutexLocker locker(&mutex);
                condition.wait(&mutex);
            }
            if (state == State::StoppedState)
            {
                qInfo() << "Forcing the kernel to stop...";
                return;
            }
        }

        auto state = tasks->getState(task);
        if (state == Task::State::Timeout)
        {
            qInfo() << "Timeout happened. Removing the current ready task...";
            readyTasks->removeBest();

            bool ok = false;
            auto usedQuantums = tasks->data(task, Task::QuantumRole).toLongLong(&ok);
            auto currentPriority = tasks->data(task, Task::PriorityRole).toLongLong(&ok);
            if (!ok)
            {
                qWarning() << "Failed to retrieve the current values: Current Priority:" << currentPriority << "Used Quantums:" << usedQuantums;
                usedQuantums = 0;
                currentPriority = 1;
            }

            auto priority = qint64(currentPriority / (0.5 * usedQuantums + 1));
            qWarning() << "The new priority is " << priority;

            tasks->setData(task, priority, Task::PriorityRole);
            {
                auto result = tasks->setState(task, Task::State::Ready);
                if (result.successful)
                {
                    qInfo() << "Inserting the task again into the ready queue...";
                    readyTasks->insertTask(task);

                    qInfo() << "Logging the status...";
                    logTask(task, result.previous, result.current);
                }
                else
                {
                    qDebug() << "Failed to set the new state...";
                }
            }
        }
        else
        {
            qInfo() << "No timeout happened.";
            bool ok = false;
            auto time = task.data(Task::RemainingTimeRole).toLongLong(&ok);
            if (!ok)
            {
                qInfo() << "Failed to retrieve the remaining time... Time:" << time;
                time = 0;
            }
            if (time == 0)
            {
                qInfo() << "Task has been finished...Removing the task from the ready qeueus...";
                readyTasks->removeBest();

                qInfo() << "Logging the status.";
                logTask(task, tasks->getState(task), Task::State::Terminate);

                qInfo() << "Removing the task from the whole list...";
                if(!tasks->removeTask(task))
                {
                    qDebug() << "Failed to remove the task.";
                }
                else
                {
                    qInfo() << "Checking if there are some new tasks...";
                    while (limitTasks->rowCount() > 0 && readyTasks->hasCapacity())
                    {
                        auto newTask = limitTasks->peekBest();
                        qInfo() << "Found one size limit task:" << task.data(Task::NameRole).toString();
                        {
                            auto result = tasks->setState(newTask, Task::State::Ready);
                            if (result.successful)
                            {
                                qInfo() << "Inserting into the ready tasks.";
                                readyTasks->insertTask(newTask);

                                qInfo() << "Removing from the size limit tasks.";
                                limitTasks->removeBest();

                                qInfo() << "Logging the status...";
                                logTask(newTask, result.previous, result.current);
                            }
                            else
                            {
                                qDebug() << "Failed to change the state to ready.";
                            }
                        }
                    }
                }
            }
            else
            {
                qInfo() << "Logging the status...Seems it just got executed.";
                logTask(task, tasks->getState(task), Task::State::Execute);
            }
        }
    }
    QMutexLocker locker(&mutex);
    if (shudownSchedule)
    {
        qInfo() << "Scheduled for a shutdown...Removing everything...";
        readyTasks->clear();
        limitTasks->clear();
        agentTasks->clear();
        tasks->removeTask(QModelIndex());
    }
}
