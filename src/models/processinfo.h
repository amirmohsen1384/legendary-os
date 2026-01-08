#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <QString>
#include <memory>
#include <QList>

class ProcessInfo
{
public:
    enum class State {Running, Ready, WaitingForFile, WaitingForLimit};

public:
    ProcessInfo(ProcessInfo *parent = nullptr);

    qint64 getIdentifier() const;

    void setIdentifier(qint64 value);

    QString getName() const;

    void setName(const QString &value);

    qint64 getPriority() const;

    void setPriority(qint64 value);

    qint64 getBurstTime() const;

    void setBurstTime(qint64 value);

    State getState() const;

    void setState(State value);

    bool needsFile() const;

    QString getFileName() const;

    void setFileName(const QString &value);

    qint64 getStartTime() const;

    void setStartTime(qint64 value);

    qint64 getFinishTime() const;

    void setFinishTime(qint64 value);

    ProcessInfo* getParent() const;

    void setParent(ProcessInfo *value);

    void addSubprocess(std::unique_ptr<ProcessInfo> process);

    qint64 subProcessCount() const;

    ProcessInfo* subProcess(int row);

    qint64 row() const;

    qint64 columnCount() const;

private:
    State state;
    QString name;
    qint64 priority;
    QString fileName;
    qint64 burstTime;
    qint64 startTime;
    qint64 finishTime;
    qint64 identifier;
    ProcessInfo *parent = nullptr;
    std::vector<std::unique_ptr<ProcessInfo>> children;
};

#endif // PROCESSINFO_H
