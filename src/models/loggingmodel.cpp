#include "loggingmodel.h"
#include <QJsonDocument>
#include <QJsonObject>

LoggingModel::LoggingModel(QObject *parent) : QAbstractTableModel(parent)
{}

QVariant LoggingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(orientation)
    {
    case Qt::Vertical:
    {
        return section + 1;
    }
    case Qt::Horizontal:
    {
        switch(role)
        {
        case Qt::DisplayRole:
        {
            auto group = static_cast<Header>(section);
            switch (group)
            {
            case Header::ID:
            {
                return "ID";
            }
            case Header::Name:
            {
                return "Name";
            }
            case Header::Agent:
            {
                return "Agent";
            }
            case Header::Timestamp:
            {
                return "Timestamp";
            }
            case Header::Current:
            {
                return "Current State";
            }
            case Header::Previous:
            {
                return "Previous State";
            }
            }
        }
        case Qt::TextAlignmentRole:
        {
            return Qt::AlignCenter;
        }
        default:
        {
            return {};
        }
        }
    }
    default:
    {
        return {};
    }
    }
}

QByteArray LoggingModel::report() const
{
    QJsonDocument document;
    QJsonObject object;
    for (const LogInfo &info : container)
    {
        const auto key = info.name;
        QJsonObject value;
        value.insert("timestamp", info.timestamp);
        value.insert("id", info.identifier);
        if (info.agent.isValid())
        {
            QJsonObject agent;
            agent.insert("name", info.agent.getName());
            agent.insert("description", info.agent.getDescription());
            value.insert("agent", agent);
        }
        value.insert("current", LogInfo::text(info.current));
        value.insert("previous", LogInfo::text(info.previous));
        if (!info.description.isEmpty())
        {
            value.insert("note", info.description);
        }
        object.insert(key, value);
    }
    document.setObject(object);
    return document.toJson();
}

bool LoggingModel::log(const LogInfo &info)
{
    beginInsertRows(QModelIndex(), container.size(), container.size());
    container.append(info);
    endInsertRows();
    return true;
}

void LoggingModel::clear()
{
    beginResetModel();
    container.clear();
    endResetModel();
}

int LoggingModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return container.size();
}

int LoggingModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    auto count = 0;

    // Timestamp
    ++count;

    // ID
    ++count;

    // Name
    ++count;

    // Agent
    ++count;

    // Previous State
    ++count;

    // Current State
    ++count;

    return count;
}

QVariant LoggingModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return {};
    }
    const auto row = index.row();
    const auto &data = container[row];
    switch (role)
    {
    case Qt::DisplayRole:
    {
        auto group = static_cast<Header>(index.column());
        switch (group)
        {
        case Header::ID:
        {
            return data.identifier;
        }
        case Header::Name:
        {
            return data.name;
        }
        case Header::Agent:
        {
            return data.agent.isValid() ? data.agent.getName() : "No Agent";
        }
        case Header::Timestamp:
        {
            return data.timestamp;
        }
        case Header::Current:
        {
            return LogInfo::text(data.previous);
        }
        case Header::Previous:
        {
            return LogInfo::text(data.current);
        }
        default:
        {
            return {};
        }
        }
        break;
    }
    case Qt::TextAlignmentRole:
    {
        return Qt::AlignCenter;
    }
    case Qt::ForegroundRole:
    {
        auto group = static_cast<Header>(index.column());
        switch (group)
        {
        case Header::Current:
        {
            return LogInfo::color(data.current);
        }
        case Header::Previous:
        {
            return LogInfo::color(data.previous);
        }
        default:
        {
            return {};
        }
        }
    }
    case Qt::ToolTipRole:
    {
        return data.description;
    }
    case Qt::UserRole:
    {
        return QVariant::fromValue(data);
    }
    default:
    {
        return {};
    }
    }
}

QString LogInfo::text(const State &info)
{
    switch (info)
    {
    case State::Error:
    {
        return "Error happened";
    }
    case State::Executed:
    {
        return "Executed";
    }
    case State::Ready:
    {
        return "Ready";
    }
    case State::Running:
    {
        return "Running";
    }
    case State::Terminated:
    {
        return "Finished";
    }
    case State::Unknown:
    {
        return "Not Known";
    }
    case State::WaitingForFile:
    {
        return "Waiting for file";
    }
    case State::WaitingForLimit:
    {
        return "Waiting for limit";
    }
    default:
    {
        return QString();
    }
    }
}

QColor LogInfo::color(const State &info)
{
    switch(info)
    {
    case LogInfo::Error:
    {
        return QColor(Qt::darkRed);
    }
    case LogInfo::Executed:
    {
        return QColor(Qt::darkBlue);
    }
    case LogInfo::Terminated:
    {
        return QColor(Qt::darkMagenta);
    }
    case LogInfo::Ready:
    {
        return QColor(Qt::darkCyan);
    }
    case LogInfo::Running:
    {
        return QColor(Qt::darkGreen);
    }
    default:
    {
        return {};
    }
    }
}
