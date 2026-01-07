#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <QString>
#include <memory>
#include <QList>

class ProcessInfo
{
public:
    enum class Command
    {
        CreateProcess,
        DeleteProcess,
        CreateFile,
        DeleteFile,
        Shutdown
    };

    enum class State {Running, Ready, WaitingForFile, WaitingForLimit};

public:
    ProcessInfo(ProcessInfo *parent = nullptr);

    qint64 getIdentifier() const;

    void setIdentifier(qint64 value);

    Command getCommand() const;

    void setCommand(Command value);

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

private:
    State state;
    Command command;
    qint64 priority;
    QString fileName;
    qint64 burstTime;
    qint64 startTime;
    qint64 finishTime;
    qint64 identifier;
    ProcessInfo *parent = nullptr;
    QList<std::unique_ptr<ProcessInfo>> children;
};

#endif // PROCESSINFO_H
