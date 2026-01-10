#ifndef PERSONMODEL_H
#define PERSONMODEL_H

#include <QAbstractItemModel>
#include "person.h"

class PersonModel : public QAbstractItemModel
{
    Q_OBJECT

    std::unique_ptr<Person> createPerson(const PersonInfo &info, Person *parent);

public:
    enum class Header {Name = 0, Birthday = 1};

    explicit PersonModel(QObject *parent = nullptr);

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &index) const override;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    virtual void insert(const PersonInfo &data, const QModelIndex &parent);
    virtual void remove(const QModelIndex &index);

private:
    std::unique_ptr<Person> root;
};

#endif // PERSONMODEL_H
