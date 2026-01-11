#include <QApplication>
#include "core/system.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Config::initialize();

    auto config = Config::load();
    return app.exec();
}
