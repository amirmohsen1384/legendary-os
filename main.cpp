#include <QMessageBox>
#include <QApplication>
#include "core/system.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Config::Container settings;
    try
    {
        Storage::initialize();
        if (!Config::exists())
        {
            throw Exception::ConfigNotFound();
        }
        settings = Config::load();
    }
    catch (const Exception::ConfigNotFound &error)
    {
        QMessageBox::information(
            nullptr,
            "No configuration found",
            QString("%1\n\nWe have initialized the system with default data.").arg(error.what())
        );
    }
    catch (const Exception::ConfigCorruption &error)
    {
        QMessageBox::warning(
            nullptr,
            "Configuration corrupted",
            QString("%1\n\nThe configuration file will be reset back to the default.").arg(error.what())
        );
    }
    catch(...)
    {
        QMessageBox::critical(
            nullptr,
            "Unknown Error",
            "An unknown error has happened. The application will be closed."
        );
        QApplication::quit();
    }

    return app.exec();
}
