#ifndef PROCESSPANEL_H
#define PROCESSPANEL_H

#include <QDialog>
#include "models/processmodel.h"

namespace Ui
{
    class ProcessPanel;
}

class ProcessPanel : public QDialog
{
    Q_OBJECT
public:
    explicit ProcessPanel(ProcessModel* model, QWidget *parent = nullptr);
    ~ProcessPanel();

    QString getName() const;
    void setName(const QString &value);

    qint64 getPriority() const;
    void setPriority(qint64 value);

    qint64 getBurstTime() const;
    void setBurstTime(qint64 value);

    QString getFileName() const;
    void setFileName(const QString &value);

    bool dependsOnFile() const;
    void setDependsOnFile(bool value);

    ProcessInfo getProcessInfo() const;
    void setProcessInfo(const ProcessInfo &info);

    QModelIndex getParent() const;
    void setParent(const QModelIndex &parent);

    ProcessModel* getModel();
    void setModel(ProcessModel *model);

    void expandFrom(const QModelIndex &index);

private:
    std::unique_ptr<Ui::ProcessPanel> ui;
};

#endif // PROCESSPANEL_H
