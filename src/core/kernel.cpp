#include "kernel.h"
#include <QThread>

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

}

void CentralKernel::removeTask(const QModelIndex &index)
{

}

void CentralKernel::addAgent(const AgentInfo &info, const QModelIndex &parent)
{

}

void CentralKernel::removeAgent(const QModelIndex &index)
{

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
