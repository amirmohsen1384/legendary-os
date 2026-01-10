#include "personmodel.h"

PersonModel::PersonModel(QObject *parent) : QAbstractItemModel(parent)
{
    root = std::make_unique<Person>(nullptr);
}

QModelIndex PersonModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return {};
    }
    auto ancestor = parent.isValid() ? static_cast<Person*>(parent.internalPointer()) : root.get();
    if (!ancestor)
    {
        ancestor = root;
    }
    auto item = ancestor->getChild(row);
    return !item ? QModelIndex() : createIndex(row, 0, item);
}

QModelIndex PersonModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
    {
        return {};
    }
    auto item = static_cast<Person*>(index.internalPointer());
    auto parent = item->getParent();
    if (parent == nullptr || parent == root.get())
    {
        return {};
    }
    else
    {
        return createIndex(parent->getRow(), 0, parent);
    }
}

int PersonModel::rowCount(const QModelIndex &parent) const
{
    auto ancestor = !parent.isValid() ? root.get() : static_cast<Person*>(parent.internalPointer());
    return !ancestor ? 0 : ancestor->childCount();
}

QVariant PersonModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
    {
        return {};
    }
    switch (role)
    {
    case Qt::DisplayRole:
    {
        auto group = static_cast<PersonModel::Header>(section);
        switch(group)
        {
        case PersonModel::Header::Name:
        {
            return "Name";
        }
        case PersonModel::Header::Job:
        {
            return "Job";
        }
        case PersonModel::Header::ID:
        {
            return "ID";
        }
        case PersonModel::Header::Birthday:
        {
            return "Birthday";
        }
        default:
        {
            return {};
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

int PersonModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
    {
        return 0;
    }

    int count = 0;

    // Name
    count++;

    // Birthday
    count++;

    // Job
    count++;

    // Identifier
    count++;

    return count;
}

QVariant PersonModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    auto item = static_cast<Person*>(index.internalPointer());
    switch (role)
    {
    case Qt::DisplayRole:
    {
        auto group = static_cast<PersonModel::Header>(index.column());
        switch(group) {
        case PersonModel::Header::Name:
        {
            return item->getFullName();
        }
        case PersonModel::Header::Job:
        {
            switch(item->getJob())
            {
            case Person::Job::CEO:
            {
                return "Chief Executive Officer";
            }
            case Person::Job::Accountant:
            {
                return "Accountant";
            }
            case Person::Job::ContentCreator:
            {
                return "Content Creator";
            }
            case Person::Job::Manager:
            {
                return "Manager";
            }
            case Person::Job::ProductManager:
            {
                return "Product Manager";
            }
            case Person::Job::ProductOwner:
            {
                return "Product Owner";
            }
            case Person::Job::PromptEngineer:
            {
                return "Prompt Engineer";
            }
            case Person::Job::SoftwareDeveloper:
            {
                return "Software Developer";
            }
            case Person::Job::Supervisor:
            {
                return "Supervisor";
            }
            case Person::Job::Unknown:
            {
                return "Not Known";
            }
            default:
            {
                return {};
            }
            }
        }
        case PersonModel::Header::ID:
        {
            return item->getIdentifier();
        }
        case PersonModel::Header::Birthday:
        {
            return item->getBirthday().toString();
        }
        default:
        {
            return {};
        }
    }
    case Qt::DecorationRole:
    {
        const auto &photo = item->getPhoto();
        return photo.isNull() ? QPixmap() : photo.scaled(64, 64);
    }
    }
}
