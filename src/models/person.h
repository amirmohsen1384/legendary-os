#ifndef PERSON_H
#define PERSON_H

#include <QString>
#include <QPixmap>
#include <QDate>

class Person
{
public:
    enum Role
    {
        NameRole = Qt::DisplayRole,
        FirstNameRole = Qt::UserRole,
        LastNameRole = Qt::UserRole + 1,
        BirthdayRole = Qt::UserRole + 2,
        BiographyRole = Qt::UserRole + 3
    };

public:
    Person(Person *parent = nullptr);

    QString getBiography() const;

    QString getFirstName() const;

    QString getLastName() const;

    QString getFullName() const;

    QDate getBirthday() const;

    Person* getChild(int row);

    qint64 childCount() const;

    qint64 getColumn() const;

    qint64 getRow() const;

    Person* getParent();

public:
    void addChild(std::unique_ptr<Person> item);

    void setBiography(const QString &value);

    void setFirstName(const QString &value);

    void setLastName(const QString &value);

    void setBirthday(const QDate &value);

    void setParent(Person* value);

    void removeChild(int row);

private:
    std::vector<std::unique_ptr<Person>> children;
    Person* parent = nullptr;
    QString biography;
    QString firstName;
    QString lastName;
    QDate birthday;
};

#endif // PERSON_H
