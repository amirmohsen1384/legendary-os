#ifndef LOGGINGMODEL_H
#define LOGGINGMODEL_H

#include <QAbstractTableModel>
#include "agent.h"

struct LogInfo
{
    enum State
    {
        Unknown,
        Ready,
        Error,
        Running,
        Executed,
        Terminated,
        WaitingForFile,
        WaitingForLimit
    };
    QString name;
    AgentInfo agent;
    qint64 timestamp;
    qint64 identifier;
    QString description;
    State current = State::Unknown;
    State previous = State::Unknown;
    static QString text(const State &info);
    static QColor color(const State &info);
};

Q_DECLARE_METATYPE(LogInfo)

class LoggingModel : public QAbstractTableModel
{
    Q_OBJECT
    enum class Header {Timestamp, Name, Agent, ID, Current, Previous};

public:
    explicit LoggingModel(QObject *parent = nullptr);

public:
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public:
    QByteArray report() const;

public slots:
    bool log(const LogInfo &info);
    void clear();

private:
    QList<LogInfo> container;
};

#endif // LOGGINGMODEL_H
