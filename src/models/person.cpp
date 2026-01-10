#include "person.h"

Person::Person(Person *parent)
{
    this->parent = parent;
}

QString PersonInfo::getFirstName() const
{
    return firstName;
}

QString PersonInfo::getLastName() const
{
    return lastName;
}

QString PersonInfo::getFullName() const
{
    return QString("%1 %2").arg(firstName, lastName);
}

Person *Person::getChild(int row)
{
    if (row < 0 || row >= children.size())
    {
        return nullptr;
    }
    return children.at(row).get();
}

QDate PersonInfo::getBirthday() const
{
    return birthday;
}

qint64 Person::childCount() const
{
    return children.size();
}

qint64 Person::columnCount() const
{
    int count = 0;

    // Name
    count++;

    // Birthday
    count++;

    return count;
}

QString PersonInfo::getBiography() const
{
    return biography;
}

qint64 Person::getRow() const
{
    if (!parent)
    {
        return 0;
    }
    else
    {
        const auto &container = parent->children;
        auto result = std::find_if(container.cbegin(), container.cend(),
            [&](const std::unique_ptr<Person> &item)
            {
                if (item.get() == this)
                {
                    return true;
                }
                return false;
            }
        );
        if (result != container.cend())
        {
            return std::distance(container.cbegin(), result);
        }
        else
        {
            Q_ASSERT(false);
            return -1;
        }
    }
}

Person *Person::getParent()
{
    return parent;
}

Person *Person::find(const QString &name)
{
    for(const auto &child : children)
    {
        if (child->getFullName() == name)
        {
            return child.get();
        }
    }
    return nullptr;
}

void PersonInfo::setFirstName(const QString &value)
{
    firstName = value;
}

void PersonInfo::setLastName(const QString &value)
{
    lastName = value;
}

void PersonInfo::setBirthday(const QDate &value)
{
    birthday = value;
}

void Person::setParent(Person *value)
{
    parent = value;
}

void PersonInfo::setBiography(const QString &value)
{
    biography = value;
}

void Person::addChild(std::unique_ptr<Person> item)
{
    if (item)
    {
        children.push_back(std::move(item));
    }
}

void Person::removeChild(int row)
{
    if (row < 0 || row >= children.size())
    {
        return;
    }
    children.erase(children.begin() + row);
}
