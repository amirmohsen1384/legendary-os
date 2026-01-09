#ifndef PERSON_H
#define PERSON_H

#include <QString>
#include <QPixmap>
#include <QDate>

class Person
{
public:
    enum class Role {
        Unknown = 0,
        CEO,
        Manager,
        Supervisor,
        Accountant,
        ProductOwner,
        ProductManager,
        ContentCreator,
        PromptEngineer,
        SoftwareDeveloper
    };

public:
    Person(Person *parent = nullptr);

    Person::Role getRole() const;

    qint64 getIdentifier() const;

    QString getFirstName() const;

    QString getLastName() const;

    QString getFullName() const;

    Person* getChild(int row);

    QPixmap getPhoto() const;

    qint64 getSalary() const;

    QDate getBirthday() const;

    qint64 childCount() const;

    qint64 getRow() const;

    Person* getParent();

public:
    void setRole(Person::Role value);

    void setIdentifier(qint64 value);

    void setFirstName(const QString &value);

    void setLastName(const QString &value);

    void setPhoto(const QPixmap &value);

    void setSalary(qint64 value);

    void setBirthday(const QDate &value);

    void setParent(Person* value);

    void addChild(std::unique_ptr<Person> item);

    void removeChild(int row);

private:
    std::vector<std::unique_ptr<Person>> children;
    Person::Role role = Person::Role::Unknown;
    Person* parent = nullptr;
    qint64 identifier;
    QString firstName;
    QString lastName;
    QDate birthday;
    qint64 salary;
    QPixmap photo;
};

#endif // PERSON_H
