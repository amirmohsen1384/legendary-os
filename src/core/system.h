#ifndef SYSTEM_H
#define SYSTEM_H

#include <QString>

namespace Exception
{
    class ConfigNotFound : public std::exception
    {
    public:
        const char* what() const noexcept override;
    };

    class ConfigCorruption : public std::exception
    {
    public:
        const char* what() const noexcept override;
        QString error;
    };
}

namespace Config
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
    Config::Info load();
    void save(const Config::Info &info);
}

#endif // SYSTEM_H
