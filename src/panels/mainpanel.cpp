#include "mainpanel.h"
#include "ui_mainpanel.h"

MainPanel::MainPanel(QWidget *parent) : QMainWindow(parent)
{
    ui = std::make_unique<Ui::MainPanel>();
    ui->setupUi(this);
}

MainPanel::~MainPanel() {}
