#include "coordinator.h"
#include <QMutexLocker>

Coordinator::Coordinator(const Config::Info &info, QObject *parent)
    : QThread(parent), settings(info)
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
    if (!index.isValid()) {
        return false;
    }
    const auto currentPath = agents->toString(index);
    for (auto i = 0; i < agentTasks->rowCount(); ++i) {
        auto target = agentTasks->index(i, 0);
        auto targetPath = agentTasks->data(target, Qt::UserRole).value<QModelIndex>().data(Task::AgentRole).toString();
        if (targetPath == currentPath)
        {
            agentTasks->removeRow(i--);
            if (readyTasks->hasCapacity())
            {
                tasks->setData(target, Task::State::Ready, Task::StateRole);
                readyTasks->insertTask(target);
            }
            else
            {
                tasks->setData(target, Task::State::WaitingForLimit, Task::StateRole);
                limitTasks->insertTask(target);
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

bool Coordinator::removeAgent(const QModelIndex &index)
{
    for (auto i = 0; i < agents->rowCount(index); ++i) {
        auto index = agents->index(i, 0, index);
        if (!removeAgent(index)) {
            return false;
        }
    }
    auto path = agents->toString(index);
    for (auto i = 0; i < readyTasks->rowCount(); ++i) {
        auto index = readyTasks->index(i, 0);
        auto currentPath = index.data(Qt::UserRole).value<QModelIndex>().data(Task::AgentRole).toString();
        if (path == currentPath) {
            readyTasks->removeRow(i--);
            agentTasks->insertTask(index);
        }
    }
    for (auto i = 0; i < limitTasks->rowCount(); ++i) {
        auto index = readyTasks->index(i, 0);
        auto currentPath = index.data(Qt::UserRole).value<QModelIndex>().data(Task::AgentRole).toString();
        if (path == currentPath) {
            readyTasks->removeRow(i--);
            agentTasks->insertTask(index);
        }
    }
    return agents->removeAgent(index);
}

bool Coordinator::removeTask(const QModelIndex &index)
{
    for (auto i = 0; i < tasks->rowCount(index); ++i) {
        auto index = tasks->index(i, 0, index);
        if (!removeTask(index)) {
            return false;
        }
    }
    for (auto i = 0; i < readyTasks->rowCount(); ++i) {
        auto index = readyTasks->index(i, 0);
        auto current = index.data(Qt::UserRole).value<QModelIndex>();
        if (index == current) {
            readyTasks->removeRow(i--);
        }
    }
    for (auto i = 0; i < limitTasks->rowCount(); ++i) {
        auto index = limitTasks->index(i, 0);
        auto current = index.data(Qt::UserRole).value<QModelIndex>();
        if (index == current) {
            limitTasks->removeRow(i--);
        }
    }
    for (auto i = 0; i < agentTasks->rowCount(); ++i) {
        auto index = agentTasks->index(i, 0);
        auto current = index.data(Qt::UserRole).value<QModelIndex>();
        if (index == current) {
            agentTasks->removeRow(i--);
        }
    }
    return tasks->removeTask(index);
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
            tasks->setData(task, Task::State::Ready, Task::StateRole);
            readyTasks->insertTask(task);
        }
        else
        {
            tasks->setData(task, Task::State::WaitingForLimit, Task::StateRole);
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
                tasks->setData(task, Task::State::Ready, Task::StateRole);
                readyTasks->insertTask(task);
            }
            else
            {
                tasks->setData(task, Task::State::WaitingForLimit, Task::StateRole);
                limitTasks->insertTask(task);
            }
        }
        else
        {
            tasks->setData(task, Task::State::WaitingForAgent, Task::StateRole);
            agentTasks->insertTask(task);
        }
    }
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
    if (!hasReqiurements()) {
        return;
    }
    const auto unit = settings.quantumSize;
    for (auto i = 0; i < settings.executionCycle; ++i)
    {
        const auto current = getElapsedQuantums();

        if (readyTasks->rowCount() > 0)
        {
            auto task = readyTasks->peekBest();

            tasks->beginToProceed(task, current);
            QThread::sleep(settings.pause);

            tasks->proceed(task, unit);
            QThread::sleep(settings.pause);

            current += unit;
            setElapsedQuantums(current);
            tasks->endToProceed(task, current);
            QThread::sleep(settings.pause);

            auto state = tasks->data(task, Task::StateRole).value<Task::State>();
            if (state == Task::State::Timeout)
            {
                readyTasks->removeBest();
                bool ok = false;
                auto usedQuantums = tasks->data(task, Task::QuantumRole).toLongLong(&ok);
                auto currentPriority = tasks->data(task, Task::PriorityRole).toLongLong(&ok);
                if (!ok) {
                    usedQuantums = 0;
                    currentPriority = 1;
                }
                auto priority = qint64(currentPriority / (0.5 * usedQuantums + 1));
                tasks->setData(task, priority, Task::PriorityRole);
                tasks->setData(task, Task::State::Ready, Task::State);
                readyTasks->insertTask(task);
            }
            bool ok = false;
            auto time = task.data(Task::RemainingTimeRole).toLongLong(&ok);
            if (!ok) {
                time = 0;
            }
            if (time == 0) {
                if(!tasks->removeTask(task)) {
                    qDebug() << "Failed to remove the task.";
                }
                else
                {
                    while (limitTasks->rowCount() > 0 && readyTasks->hasCapacity()) {
                        auto newTask = limitTasks->peekBest();
                        limitTasks->removeBest();

                        tasks->setData(newTask, Task::State::Ready, Task::StateRole);
                        readyTasks->insertTask(newTask);
                    }
                }
            }
        }
        else
        {
            QMutexLocker locker(&mutex);
            unusedQuantums += unit;
            emit quantumUnused(unusedQuantums);
        }
    }
    if (shudownSchedule)
    {
        tasks->removeTask(QModelIndex());
        readyTasks->clear();
        limitTasks->clear();
        agentTasks->clear();
    }
}
