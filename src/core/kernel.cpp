#include "kernel.h"

bool Kernel::dispatch(const QModelIndex &index)
{
    if (!index.isValid())
    {
        qDebug() << "Index is invalid.";
        return false;
    }

    auto executable = index.data(Task::ExecutableRole).toBool();
    if (executable)
    {
        if (readyTasks->hasCapacity())
        {
            if (tasks->setData(index, Task::State::Ready, Task::StateRole))
            {
                readyTasks->insertTask(index);
                return true;
            }
            else
            {
                qDebug() << "Failed to push the task into ready queue.";
            }
        }
        else
        {
            if(tasks->setData(index, Task::State::WaitingForLimit, Task::StateRole))
            {
                restTasks->insertTask(index);
                return true;
            }
            else
            {
                qDebug() << "Failed to push the task into rest queue.";
            }
        }
    }
    else
    {
        auto path = index.data(Task::AgentRole).toString();
        if (agents->index(path).isValid())
        {
            if (readyTasks->hasCapacity())
            {
                if (tasks->setData(index, Task::State::Ready, Task::StateRole))
                {
                    readyTasks->insertTask(index);
                    return true;
                }
                else
                {
                    qDebug() << "Failed to push the task into ready queue.";

                }
            }
            else
            {
                if(tasks->setData(index, Task::State::WaitingForLimit, Task::StateRole))
                {
                    restTasks->insertTask(index);
                    return true;
                }
                else
                {
                    qDebug() << "Failed to push the task into rest queue.";

                }
            }
        }
        else
        {
            if (tasks->setData(index, Task::State::WaitingForAgent, Task::StateRole))
            {
                restTasks->insertTask(index);
                return true;
            }
            else
            {
                qDebug() << "Failed to push the task into rest queue.";
            }
        }
    }
    return false;
}

Kernel::Kernel(QObject *parent) : QObject{parent}
{}

bool Kernel::hasRequirements()
{
    return agents && readyTasks && logger && agents && tasks;
}

bool Kernel::insertAgent(const AgentInfo &info, const QModelIndex &parent)
{
    if (!hasRequirements())
    {
        return false;
    }
    auto index = agents->insert(info, parent).isValid();
    auto path = agents->toString(index);
    for (auto i = 0; i < restTasks->rowCount(); ++i)
    {
        auto target = restTasks->index(i, 0);
        auto desired = target.data(Qt::UserRole).value<QPersistentModelIndex>().data(Task::AgentRole).toString();
        if (desired == path)
        {
            restTasks->removeRow(i);
            dispatch(target);
        }
    }
}

bool Kernel::insertTask(const TaskInfo &info, const QModelIndex &parent)
{
    if (!hasRequirements())
    {
        return false;
    }
    auto index = tasks->insertTask(info, parent);
    return dispatch(index);
}

bool Kernel::removeAgent(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return false;
    }
    auto path = agents->toString(index);
    for (auto i = 0; i < readyTasks->rowCount(); ++i)
    {
        if(agents->toString(readyTasks->index(i, 0)) == path)
        {
            readyTasks->removeTask(i);
        }
    }
    for (auto i = 0; i < restTasks->rowCount(); ++i)
    {
        if(agents->toString(restTasks->index(i, 0)) == path)
        {
            restTasks->removeTask(i);
        }
    }
    return agents->remove(index);
}

bool Kernel::removeTask(const QModelIndex &index)
{
    if (!hasRequirements() || !index.isValid())
    {
        return false;
    }
    for (auto i = 0; i < tasks->rowCount(index); ++i)
    {
        if(!removeTask(tasks->index(i, 0, index)))
        {
            return false;
        }
    }
    readyTasks->removeTask(index);
    restTasks->removeTask(index);
    tasks->removeTask(index);
    return true;
}

bool Kernel::scheduleShutdown()
{
    if (!shutdown)
    {
        shutdown = true;
        emit shutdownScheduled();
        return true;
    }
    else
    {
        return false;
    }
}
