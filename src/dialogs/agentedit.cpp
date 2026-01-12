#include "agentedit.h"
#include "ui_agentedit.h"

AgentEdit::AgentEdit(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AgentEdit)
{
    ui->setupUi(this);
}

AgentEdit::~AgentEdit()
{
    delete ui;
}
