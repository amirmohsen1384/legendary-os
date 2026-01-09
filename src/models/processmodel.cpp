#include <QRandomGenerator64>
#include "processmodel.h"
#include <QColor>

Process* ProcessModel::createProcess(const ProcessInfo &info, Process *parent)
{
    if (!parent) {
        return {};
    }

    auto process = std::make_unique<Process>(parent);
    process->setName(info.getName());
    process->setPriority(info.getPriority());
    process->setFileName(info.getFileName());
    process->setBurstTime(info.getBurstTime());

    auto pid = QRandomGenerator64::global()->bounded(0, 10000 - 1);
    process->setIdentifier(pid);

    auto result = process.get();
    parent->addChild(std::move(process));

    return result;
}

ProcessModel::ProcessModel(QObject *parent) : QAbstractItemModel(parent)
{
    root = std::make_unique<Process>(nullptr);
}

QVariant ProcessModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
    {
        return {};
    }
    switch(role)
    {
    case Qt::DisplayRole:
    {
        auto group = static_cast<Info>(section);
        switch (group)
        {
        case Info::PID: {
            return QString("PID");
        }
        case Info::Name: {
            return QString("Name");
        }
        case Info::State: {
            return QString("State");
        }
        case Info::Dependency: {
            return QString("Dependency");
        }
        case Info::Priority: {
            return QString("Priority");
        }
        default: {
            return {};
        }
        }
        break;
    }
    case Qt::BackgroundRole: {
        return QColor(220, 220, 220); // Light Gray
    }
    case Qt::TextAlignmentRole: {
        return Qt::AlignCenter;
    }
    default: {
        return {};
    }
    }
}

QModelIndex ProcessModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent)) {
        return {};
    }

    auto ancestor = parent.isValid() ?
        static_cast<Process*>(parent.internalPointer()) :
        root.get();

    if(!ancestor) {
        return {};
    }

    auto result = ancestor->getChild(row);
    return !result ? QModelIndex() : createIndex(row, column, result);
}

QModelIndex ProcessModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return {};

    } else {
        auto item = static_cast<Process*>(index.internalPointer());
        auto parent = item->getParent();
        if (parent == nullptr || parent == root.get()) {
            return {};
        }
        return createIndex(item->row(), 0, parent);
    }
}

int ProcessModel::rowCount(const QModelIndex &parent) const
{
    auto item = !parent.isValid() ? root.get() : static_cast<Process*>(parent.internalPointer());
    return !item ? item->childCount() : 0;
}

int ProcessModel::columnCount(const QModelIndex &parent) const
{
    auto item = parent.isValid() ? static_cast<Process*>(parent.internalPointer()) : root.get();
    return !item ? item->columnCount() : 0;
}

QVariant ProcessModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    auto item = static_cast<Process*>(index.internalPointer());
    if(!item) {
        return {};
    }

    switch (role) {
    case Qt::DisplayRole: {
        auto group = static_cast<Info>(index.column());
        switch (group)
        {
        case Info::Name: {
            return item->getName();
        }
        case Info::Dependency: {
            if (item->needsFile())
            {
                const auto &data = item->getFileName();
                if (data.size() > 16) {
                    return QString("%1...").arg(data.first(16));
                } else {
                    return data;
                }
            }
            else
            {
                return "No Dependency";
            }
        }
        case Info::PID: {
            return item->getIdentifier();
        }
        case Info::Priority: {
            return item->getPriority();
        }
        case Info::State: {
            switch (item->getState()) {
            case Process::State::Running: {
                return "Running";
            }
            case Process::State::Ready: {
                return "Ready";
            }
            case Process::State::WaitingForFile:
            case Process::State::WaitingForLimit: {
                return "Waiting";
            }
            }
        }
        default: {
            return {};
        }
        }
        break;
    }
    case Qt::TextAlignmentRole: {
        return Qt::AlignCenter;
    }
    case Qt::BackgroundRole: {
        auto group = static_cast<Info>(index.column());
        switch (group) {
        case Info::State: {
            switch (item->getState()) {
            case Process::State::Running: {
                return QColor(150, 255, 180); // Light Green
            }
            case Process::State::Ready: {
                return QColor(230, 255, 150); // Light Yellow
            }
            case Process::State::WaitingForFile:
            case Process::State::WaitingForLimit: {
                return QColor(170, 230, 255); // Light Blue
            }
            }
        }
        default: {
            return {};
        }
        }
    }
    case Qt::ForegroundRole: {
        auto group = static_cast<Info>(index.column());
        switch(group) {
        case Info::Dependency: {
            if (!item->needsFile()) {
                return QColor(Qt::red);
            }
            else {
                return {};
            }
        }
        default: {
            return {};
        }
        }
    }
    case Qt::UserRole:
    {
        return QVariant::fromValue(item);
    }
    default:
    {
        return {};
    }
    }
}

bool ProcessModel::addProcess(const ProcessInfo &info, const QModelIndex &parent)
{
    auto ancestor = !parent.isValid() ? root.get() : static_cast<Process*>(parent.internalPointer());
    if(!ancestor) {
        return false;
    }
    beginInsertRows(parent, ancestor->childCount() + 1, ancestor->childCount() + 1);
    auto process = createProcess(info, ancestor);
    endInsertRows();
    return process != nullptr;
}
