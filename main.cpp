#include <QMessageBox>
#include <QApplication>
#include "core/system.h"
#include "models/processmodel.h"
#include "panels/processpanel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ProcessModel model;

    while (true)
    {
        ProcessPanel panel(&model);
        if(panel.exec() == QDialog::Accepted)
        {
            auto info = panel.getProcessInfo();
            model.insert(info);
        }
    }
    return app.exec();
}
