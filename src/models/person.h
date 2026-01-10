#ifndef PERSON_H
#define PERSON_H

#include <QString>
#include <QPixmap>
#include <QDate>

class PersonInfo
{
public:
    enum Role
    {
        NameRole = Qt::UserRole,
        FirstNameRole = Qt::UserRole + 1,
        LastNameRole = Qt::UserRole + 2,
        BirthdayRole = Qt::UserRole + 3,
        BiographyRole = Qt::UserRole + 4
    };

    QString getBiography() const;

    QString getFirstName() const;

    QString getLastName() const;

    QString getFullName() const;

    QDate getBirthday() const;

public:
    void setBiography(const QString &value);

    void setFirstName(const QString &value);

    void setLastName(const QString &value);

    void setBirthday(const QDate &value);

private:
    QString biography;
    QString firstName;
    QString lastName;
    QDate birthday;
};

class Person : public PersonInfo
{
public:
    Q_DISABLE_COPY_MOVE(Person)
    Person(Person *parent = nullptr);

    Person* getChild(int row);

    qint64 childCount() const;

    qint64 columnCount() const;

    qint64 getRow() const;

    Person* getParent();

public:
    void addChild(std::unique_ptr<Person> item);

    void setParent(Person* value);

    void removeChild(int row);

private:
    std::vector<std::unique_ptr<Person>> children;
    Person* parent = nullptr;
};

#endif // PERSON_H
