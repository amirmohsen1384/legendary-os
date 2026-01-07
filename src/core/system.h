#ifndef SYSTEM_H
#define SYSTEM_H

#include <QDir>
#include <QMap>

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

namespace Storage
{
    void initialize();
    QDir workspace();
}

namespace Config
{
    enum class Info
    {
        QuantumSize = 0,
        ExecutionPerCycle = 2,
        InputLimit = 1,
        ReadyQueueLimit = 3,
        WaitingQueueLimit = 4
    };

    using Container = QMap<Info, qint64>;

    bool exists();
    Config::Container load();
    bool save(const Config::Container &value);
}

#endif // SYSTEM_H
