#include <QApplication>
#include <QSettings>
#include <stdexcept>
#include "system.h"
#include <QIcon>

const auto pause = "pauseDuration";
const auto quantum = "quantumSize";
const auto readyLimit = "readyLimit";
const auto workspaceName = "workspace";
const auto inputLimit = "inputCommandLimit";
const auto cycle = "executionQuantumsPerCycle";

void Settings::initialize(QApplication &app)
{
    app.setWindowIcon(QIcon(":/main.ico"));
    app.setOrganizationName("Legend");
    app.setApplicationName("Legendary OS");
    app.setOrganizationDomain("legend.com");
    app.setApplicationDisplayName("Legendary OS");
    app.setApplicationVersion("1.0");
    app.setAttribute(Qt::AA_EnableHighDpiScaling);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    app.setDesktopFileName("Legendary OS");
}

Settings::Info Settings::load()
{
    QSettings settings;
    Settings::Info result;
    result.pause = settings.value(pause, 500).toLongLong();
    result.quantumSize = settings.value(quantum, 20).toLongLong();
    result.executionCycle = settings.value(cycle, 5).toLongLong();
    result.readyQueueLimit = settings.value(readyLimit, 5).toLongLong();
    result.inputCommandLimit = settings.value(inputLimit, 3).toLongLong();
    Settings::save(result);
    return result;
}

void Settings::save(const Info &info)
{
    QSettings settings;
    settings.setValue(pause, info.pause);
    settings.setValue(quantum, info.quantumSize);
    settings.setValue(cycle, info.executionCycle);
    settings.setValue(readyLimit, info.readyQueueLimit);
    settings.setValue(inputLimit, info.inputCommandLimit);
}
