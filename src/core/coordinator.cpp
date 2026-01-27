#include "coordinator.h"
#include <QMutexLocker>

Coordinator::Coordinator(const Settings::Info &info, QObject *parent) : QThread(parent), settings(info)
{
    connect(this, &QThread::started, [this]() {
        if (!this->isLocked()) {
            lock();
        }
    });
    connect(this, &QThread::finished, [this]() {
        if (this->isLocked()) {
            unlock();
        }
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
    QMutexLocker locker(&mutex);
    return elapsedQuantums > 0 ? 1 - unusedQuantums / elapsedQuantums : 0;
}

bool Coordinator::isLocked()
{
    return locked;
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
    enteredCommands++;
    if (enteredCommands == settings.inputCommandLimit)
    {
        lock();
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
    enteredCommands++;
    if (enteredCommands == settings.inputCommandLimit)
    {
        lock();
    }
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
    enteredCommands++;
    if (enteredCommands == settings.inputCommandLimit)
    {
        lock();
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
    enteredCommands++;
    if (enteredCommands == settings.inputCommandLimit)
    {
        lock();
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
    enteredCommands++;
    if (enteredCommands == settings.inputCommandLimit)
    {
        lock();
    }
}

void Coordinator::lock()
{
    setLocked(true);
}

void Coordinator::unlock()
{
    setLocked(false);
}

void Coordinator::setLocked(bool state)
{
    QMutexLocker locker(&mutex);
    locked = state;
    emit lockStateChanged(state);
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

void Coordinator::removeLater(const QModelIndex &task)
{
    removeAtEnd.enqueue(task);
}

void Coordinator::removeQueuedTasks()
{
    while (!removeAtEnd.isEmpty())
    {
        auto index = removeAtEnd.dequeue();
        qDebug() << "Removing:" << index.data(Task::NameRole).toString();
        tasks->removeTask(index);
    }
}

void Coordinator::checkForPause()
{
    auto state = getState();
    if (state == State::PausedState)
    {
        qInfo() << "Pausing the kernel...";
        QMutexLocker locker(&mutex);
        condition.wait(&mutex);
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

qint64 Coordinator::evaluatePriority(const QModelIndex &task)
{
    bool ok = false;
    auto usedQuantums = tasks->data(task, Task::QuantumRole).toLongLong(&ok);
    auto currentPriority = tasks->data(task, Task::PriorityRole).toLongLong(&ok);
    if (!ok)
    {
        currentPriority = 1;
        usedQuantums = 0;
    }
    return qint64(currentPriority / (0.5 * usedQuantums + 1));
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
    else if(tasks->rowCount() <= 0)
    {
        qDebug() << "No task is available to be finished.";
        return;
    }

    qInfo() << "Running the kernel";
    setState(Coordinator::RunningState);

    const auto unit = settings.quantumSize;
    qInfo() << "The quantum size is" << unit;

    for (auto i = 0; i < settings.executionCycle; ++i)
    {
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

        checkForPause();

        auto task = readyTasks->peekBest();
        qInfo() << "Found one ready task:" << task.data(Task::NameRole).toString();
        {
            auto before = tasks->getState(task);
            tasks->beginToProceed(task, timestamp);
            auto after = tasks->getState(task);
            qDebug() << "Started the task | Previous: " << Task::text(before) << "Current:" << Task::text(after);
            logTask(task, before, after);
        }

        QThread::msleep(settings.pause);
        checkForPause();

        tasks->proceed(task, unit);
        qDebug() << "Done part of it. This task consumed" << tasks->data(task, Task::QuantumRole).toInt() << "quantums";;

        QThread::msleep(settings.pause);
        checkForPause();

        {
            QMutexLocker locker(&mutex);
            timestamp += unit;
            elapsedQuantums = timestamp;
            emit quantumElapsed(timestamp);
        }

        {
            qDebug() << "Finishing the task...";
            auto before = tasks->getState(task);
            tasks->endToProceed(task, timestamp);
            auto after = tasks->getState(task);
            qDebug() << "Previous State: " << Task::text(before) << "Current state:" << Task::text(after);
            logTask(task, before, after);
        }

        QThread::msleep(settings.pause);
        checkForPause();

        auto state = tasks->getState(task);
        qInfo() << "The current task state is " << Task::text(state);

        if (state == Task::State::Timeout)
        {
            qInfo() << "Timeout happened. Removing the current ready task...";
            readyTasks->removeBest();

            auto priority = evaluatePriority(task);
            qWarning() << "The new priority is" << priority;

            if(tasks->setData(task, priority, Task::PriorityRole))
            {
                auto result = tasks->setState(task, Task::State::Ready);
                if (result.successful)
                {
                    qInfo() << "Inserting the task again into the ready queue...";
                    readyTasks->insertTask(task);

                    qInfo() << "Inserted successfully into the ready queue...";
                    logTask(task, result.previous, result.current);
                }
                else
                {
                    qWarning() << "Failed to set the new state...";
                }
            }
            else
            {
                qWarning() << "Failed to set the priority of the task:" << task.data(Task::NameRole).toString();
            }
        }
        else
        {
            qInfo() << "No timeout happened. The current state of the task is" << Task::text(state);
            auto ok = false;

            auto time = task.data(Task::RemainingTimeRole).toLongLong(&ok);
            qInfo() << "Remaining time is " << time;
            if (!ok)
            {
                qWarning() << "Failed to retrieve the remaining time.";
                time = 0;
            }
            if (time == 0)
            {
                qInfo() << "Task has been finished.";

                readyTasks->removeBest();
                qInfo() << "Removed the task from the ready qeueus.";

                logTask(task, state, Task::State::Terminate);

                qInfo() << "Checking if there are some new tasks...";
                while (limitTasks->rowCount() > 0 && readyTasks->hasCapacity())
                {
                    qInfo() << "Fetching new task from the limit tasks.";
                    auto newTask = limitTasks->peekBest();

                    qInfo() << "Found one size limit task:" << task.data(Task::NameRole).toString();

                    auto result = tasks->setState(newTask, Task::State::Ready);
                    if (result.successful)
                    {
                        qInfo() << "Inserting into the ready tasks.";
                        readyTasks->insertTask(newTask);

                        qInfo() << "Removing from the size limit tasks.";
                        limitTasks->removeBest();

                        logTask(newTask, result.previous, result.current);
                    }
                    else
                    {
                        qDebug() << "Failed to change the state to ready.";
                    }
                }

                qInfo() << "Scheduled for deletion:" << task.data(Task::NameRole).toString();
                removeLater(task);
            }
            else
            {
                qInfo() << "Seems it just got executed. This is a pitfall in your code since it should have been timed out.";
                logTask(task, tasks->getState(task), Task::State::Execute);
            }
        }
    }

    qDebug() << "Checking shutdown scheduler.";
    mutex.lock();
    auto state = shudownSchedule;
    mutex.unlock();

    if (state)
    {
        qInfo() << "Scheduled for a shutdown. Removing everything...";
        readyTasks->clear();
        limitTasks->clear();
        agentTasks->clear();
        removeTask(QModelIndex());
    }
    else
    {
        qInfo() << "Removing all scheduled tasks at the end.";
        removeQueuedTasks();
    }

    setState(Coordinator::StoppedState);
    qInfo() << "Finished a cycle of running the kernel.";

    QMutexLocker locker(&mutex);
    enteredCommands = 0;
    qInfo() << "Reset the commands.";
}
