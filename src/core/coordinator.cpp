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
}
