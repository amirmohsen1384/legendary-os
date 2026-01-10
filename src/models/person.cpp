#include "person.h"

Person::Person(Person *parent)
{
    this->parent = parent;
}

Person::Job Person::getJob() const
{
    return job;
}

qint64 Person::getIdentifier() const
{
    return identifier;
}

QString Person::getFirstName() const
{
    return firstName;
}

QString Person::getLastName() const
{
    return lastName;
}

QString Person::getFullName() const
{
    return QString("%1 %2").arg(firstName, lastName);
}

Person *Person::getChild(int row)
{
    if (row < 0 || row >= children.size()) {
        return nullptr;
    }
    return children.at(row).get();
}

QPixmap Person::getPhoto() const
{
    return photo;
}

qint64 Person::getSalary() const
{
    return salary;
}

QDate Person::getBirthday() const
{
    return birthday;
}

qint64 Person::childCount() const
{
    return children.size();
}

qint64 Person::getRow() const
{
    if (!parent) {
        return 0;
    }
    else
    {
        const auto &container = parent->children;
        auto result = std::find_if(container.cbegin(), container.cend(),
            [&](std::unique_ptr<Person> item)
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

void Person::setJob(Job value)
{
    job = value;
}

void Person::setIdentifier(qint64 value)
{
    identifier = value;
}

void Person::setFirstName(const QString &value)
{
    firstName = value;
}

void Person::setLastName(const QString &value)
{
    lastName = value;
}

void Person::setPhoto(const QPixmap &value)
{
    photo = value;
}

void Person::setSalary(qint64 value)
{
    salary = value;
}

void Person::setBirthday(const QDate &value)
{
    birthday = value;
}

void Person::setParent(Person *value)
{
    parent = value;
}

void Person::addChild(std::unique_ptr<Person> item)
{
    if (item) {
        children.push_back(std::move(item));
    }
}

void Person::removeChild(int row)
{
    if (row < 0 || row >= children.size()) {
        return;
    }
    children.erase(children.begin() + row);
}
