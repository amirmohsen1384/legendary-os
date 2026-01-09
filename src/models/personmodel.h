#ifndef PERSONMODEL_H
#define PERSONMODEL_H

#include <QAbstractItemModel>
#include "person.h"

class PersonModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum class Header
    {
        Name = 0,
        ID,
        Birthday,
        Role
    };
    explicit PersonModel(QObject *parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    std::unique_ptr<Person> root;
};

#endif // PERSONMODEL_H
