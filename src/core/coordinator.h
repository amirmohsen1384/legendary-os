#ifndef COORDINATOR_H
#define COORDINATOR_H

#include "models/prioritymodel.h"
#include "models/loggingmodel.h"
#include "models/readymodel.h"
#include "models/agentmodel.h"
#include "models/taskmodel.h"
#include "core/system.h"
#include <QThread>
#include <QMutex>

class Coordinator : public QThread
{
    Q_OBJECT
    Q_PROPERTY(qint64 unusedQuantums READ getUnusedQuantums NOTIFY quantumUnused)
    Q_PROPERTY(qint64 elapsedQuantums READ getElapsedQuantums NOTIFY quantumElapsed)

protected:
    bool hasReqiurements() const;

protected:
    virtual void run() override;

public:
    Coordinator(const Config::Info &info, QObject *parent);
    qint64 getElapsedQuantums();
    qint64 getUnusedQuantums();

signals:
    void quantumElapsed(qint64 quantum);
    void quantumUnused(qint64 quantum);

private:
    QMutex mutex;
    Config::Info settings;
    qint64 unusedQuantums = 0;
    qint64 elapsedQuantums = 0;
    TaskModel* tasks = nullptr;
    AgentModel* agents = nullptr;
    LoggingModel* logs = nullptr;
    ReadyModel* readyTasks = nullptr;
    PriorityModel* limitTasks = nullptr;
    PriorityModel* agentTasks = nullptr;
};

#endif // COORDINATOR_H
