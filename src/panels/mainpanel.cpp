#include "mainpanel.h"
#include "ui_mainpanel.h"

MainPanel::MainPanel(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainPanel)
{
    ui->setupUi(this);
}

MainPanel::~MainPanel()
{
    delete ui;
}
