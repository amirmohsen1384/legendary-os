#ifndef PROCESS_H
#define PROCESS_H

#include <QString>

class ProcessInfo
{
public:
    enum Role {
        Name = Qt::DisplayRole,
        Priority = Qt::UserRole + 1,
        BurstTime = Qt::UserRole + 2,
        FileName = Qt::UserRole + 3,
        State = Qt::UserRole + 4
    };

    ProcessInfo() {}

    QString getName() const;

    void setName(const QString &value);

    qint64 getPriority() const;

    void setPriority(qint64 value);

    qint64 getBurstTime() const;

    void setBurstTime(qint64 value);

    bool needsFile() const;

    QString getFileName() const;

    void setFileName(const QString &value);

private:
    QString name;
    qint64 priority;
    QString fileName;
    qint64 burstTime;
};

class Process : public ProcessInfo
{
public:
    enum class State
    {
        Unknown,
        Running,
        Ready,
        WaitingForFile,
        WaitingForLimit
    };

public:
    Process(Process *parent = nullptr);
    ~Process();

    State getState() const;

    void setState(State value);

    Process* getParent();

    void setParent(Process *value);

    void addChild(std::unique_ptr<Process> process);

    void removeChild(int row);

    qint64 childCount() const;

    Process* getChild(int row);

    qint64 row() const;

    qint64 columnCount() const;

    qint64 getStartTime() const;

    void setStartTime(qint64 value);

    qint64 getFinishTime() const;

    void setFinishTime(qint64 value);

    qint64 getIdentifier() const;

    void setIdentifier(qint64 value);

    qint64 getRemainingTime() const;

    void setRemainingTime(qint64 value);

    Process* find(qint64 value);

    static qint64 getMinimumID();

    static qint64 getMaximumID();

private:
    qint64 identifier;
    qint64 startTime = 0;
    qint64 finishTime = 0;
    qint64 remainingTime = 0;
    Process *parent = nullptr;
    State state = State::Unknown;
    std::vector<std::unique_ptr<Process>> children;
};

#endif // PROCESS_H
