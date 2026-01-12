#ifndef AGENTEDIT_H
#define AGENTEDIT_H

#include <QDialog>

namespace Ui
{
    class AgentEdit;
}

class AgentEdit : public QDialog
{
    Q_OBJECT

public:
    explicit AgentEdit(QWidget *parent = nullptr);
    ~AgentEdit();

private:
    Ui::AgentEdit *ui;
};

#endif // AGENTEDIT_H
