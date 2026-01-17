#include <QTreeView>
#include <QTableView>
#include <QApplication>
#include "src/panels/mainpanel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Config::initialize();

    auto settings = Config::load();
    MainPanel panel(settings);
    panel.show();

    return app.exec();
}
