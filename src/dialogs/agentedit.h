#ifndef AGENTEDIT_H
#define AGENTEDIT_H

#include <QDialog>
#include "models/agentmodel.h"

namespace Ui
{
    class AgentEdit;
}

class AgentEdit : public QDialog
{
    Q_OBJECT

public:
    explicit AgentEdit(AgentModel *model, QWidget *parent = nullptr);
    explicit AgentEdit(QWidget *parent = nullptr);
    ~AgentEdit();

    QString getName() const;
    void setName(const QString &value);

    QString getDescription() const;
    void setDescription(const QString &value);

    QModelIndex getParent() const;
    void setParent(const QModelIndex &index);

    AgentModel* getModel() const;
    void setModel(AgentModel *model);

    void expandFrom(const QModelIndex &index);

public:
    virtual void accept();

private:
    std::unique_ptr<Ui::AgentEdit> ui;
};

#endif // AGENTEDIT_H
