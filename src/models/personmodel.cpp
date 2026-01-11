#include "personmodel.h"

Person* PersonModel::createPerson(const PersonInfo &info, Person *parent)
{
    if (!parent)
    {
        return {};
    }
    auto person = std::make_unique<Person>(parent);
    person->setFirstName(info.getFirstName());
    person->setBiography(info.getBiography());
    person->setLastName(info.getLastName());
    person->setBirthday(info.getBirthday());

    auto result = person.get();
    parent->addChild(std::move(person));

    return result;
}

PersonModel::PersonModel(QObject *parent) : QAbstractItemModel(parent)
{
    root = std::make_unique<Person>();
    root->setFirstName("Main");
    root->setLastName("Root");
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
        ancestor = root.get();
    }
    auto item = ancestor->getChild(row);
    return !item ? QModelIndex() : createIndex(row, column, item);
}

QModelIndex PersonModel::index(const QString &path) const
{
    QStringList tokens = path.split('/');
    QStringListIterator iterator(tokens);
    if (!iterator.hasNext())
    {
        return {};
    }
    else if (!iterator.next().isEmpty())
    {
        return {};
    }
    auto target = root.get();
    while (iterator.hasNext())
    {
        const QString &name = iterator.next();
        auto result = target->find(name);
        if (!result)
        {
            return {};
        }
        target = result;
    }
    return createIndex(target->getRow(), 0, target);
}

QModelIndex PersonModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
    {
        return {};
    }
    auto item = static_cast<Person*>(index.internalPointer());
    if (!item)
    {
        return {};
    }
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
    return ancestor ? ancestor->childCount() : 0;
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
    case Qt::BackgroundRole:
    {
        return QColor(220, 220, 220);
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
    auto ancestor = !parent.isValid() ? root.get() : static_cast<Person*>(parent.internalPointer());
    return ancestor ? ancestor->columnCount() : 0;
}

QVariant PersonModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return {};
    }
    auto item = static_cast<Person*>(index.internalPointer());
    if(!item)
    {
        return {};
    }
    switch (role)
    {
    case Qt::DisplayRole:
    {
        auto group = static_cast<PersonModel::Header>(index.column());
        switch(group)
        {
        case PersonModel::Header::Name:
        {
            return item->getFullName();
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
    }
    case Qt::ToolTipRole:
    {
        const auto biography = item->getBiography();
        return biography.size() <= 24 ? biography : QString("%1...").arg(biography.first(24));
    }
    case Qt::TextAlignmentRole:
    {
        return Qt::AlignCenter;
    }
    case Person::FirstNameRole:
    {
        return item->getFirstName();
    }
    case Person::LastNameRole:
    {
        return item->getLastName();
    }
    case Person::BirthdayRole:
    {
        return item->getBirthday();
    }
    case Person::BiographyRole:
    {
        return item->getBiography();
    }
    default:
    {
        return {};
    }
    }
}

bool PersonModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto changed = false;
    if (!value.isValid())
    {
        return changed;
    }
    auto item = static_cast<Person*>(index.internalPointer());
    switch(role)
    {
    case Person::FirstNameRole:
    {
        if (value.canConvert<QString>())
        {
            item->setFirstName(value.toString());
            changed = true;
        }
        break;
    }
    case Person::LastNameRole:
    {
        if (value.canConvert<QString>())
        {
            item->setLastName(value.toString());
            changed = true;
        }
    }
    case Person::BiographyRole:
    {
        if (value.canConvert<QString>())
        {
            item->setBiography(value.toString());
            changed = true;
        }
    }
    case Person::BirthdayRole:
    {
        if (value.canConvert<QDate>())
        {
            item->setBirthday(value.toDate());
            changed = true;
        }
    }
    }
    if (changed)
    {
        emit dataChanged(index, index, {role, Qt::DisplayRole, Qt::ToolTipRole});
    }
    return changed;
}

bool PersonModel::insert(const PersonInfo &data, const QModelIndex &parent)
{
    auto ancestor = !parent.isValid() ? root.get() : static_cast<Person*>(parent.internalPointer());
    if(!ancestor)
    {
        return false;
    }
    beginInsertRows(parent, ancestor->childCount(), ancestor->childCount());
    auto person = createPerson(data, ancestor);
    endInsertRows();
    return person != nullptr;
}

bool PersonModel::remove(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return false;
    }
    const auto item = index.parent();
    auto parent = !item.isValid() ? root.get() : static_cast<Person*>(item.internalPointer());
    beginRemoveRows(item, index.row(), index.row()); // This causes some problems.
    parent->removeChild(index.row());
    endRemoveRows();
    return true;
}

QString PersonModel::toString(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return QString();
    }
    return QString("%1/%2").arg(
        toString(index.parent()),
        index.data(Person::NameRole).toString()
    );
}

void PersonModel::clear()
{
    beginResetModel();
    root.reset(nullptr);
    root = std::make_unique<Person>(nullptr);
    root->setFirstName("Main");
    root->setLastName("Root");
    endResetModel();
}
