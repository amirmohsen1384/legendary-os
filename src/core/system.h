#ifndef SYSTEM_H
#define SYSTEM_H

#include <QString>

namespace Settings
{
    struct Info
    {
        qint64 pause;
        qint64 quantumSize;
        qint64 executionCycle;
        qint64 readyQueueLimit;
        qint64 inputCommandLimit;
    };

    void initialize();
    Settings::Info load();
    void save(const Settings::Info &info);
}

#endif // SYSTEM_H
