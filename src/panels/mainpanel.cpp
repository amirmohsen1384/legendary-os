#include "mainpanel.h"
#include "ui_mainpanel.h"

MainPanel::MainPanel(const Config::Info &info, QWidget *parent) : QMainWindow(parent), settings(info)
{
    ui = std::make_unique<Ui::MainPanel>();
    ui->setupUi(this);
}

MainPanel::~MainPanel() {}

qint64 MainPanel::getElapsedQuantum() const
{
    return elapsedQuantum;
}
