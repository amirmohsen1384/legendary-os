#include <QFile>
#include "system.h"
#include <stdexcept>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

const auto quantum = "quantumSize";
const auto readyLimit = "readyLimit";
const auto configName = "config.json";
const auto workspaceName = "workspace";
const auto inputLimit = "inputCommandLimit";
const auto queueLimit = "waitingQueueLimit";
const auto cycle = "executionQuantumsPerCycle";

const char* Exception::ConfigNotFound::what() const noexcept
{
    return "No configuration file has been found.";
}

const char* Exception::ConfigCorruption::what() const noexcept
{
    return "The configuration file was corrupted or invalid. ";
}
QDir Storage::workspace()
{
    QDir workspace(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    workspace.cd(workspaceName);
    return workspace;
}

void Storage::initialize()
{
    QDir workspace(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!workspace.exists())
    {
        workspace.mkpath(workspaceName);
    }
    else
    {
        QDir current = workspace;
        workspace.cd(workspaceName);
        if (!workspace.exists())
        {
            current.mkpath(workspaceName);
        }
    }
}

bool Config::exists()
{
    return QFile::exists(Storage::workspace().absoluteFilePath(configName));
}

Config::Container Config::load()
{
    const QString fileName = Storage::workspace().absoluteFilePath(configName);
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
    {
        throw Exception::ConfigNotFound();
    }
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError)
    {
        Exception::ConfigCorruption exception;
        exception.error = error.errorString();
        throw exception;
    }

    if (!document.isObject())
    {
        Exception::ConfigCorruption exception;
        throw exception;
    }

    Config::Container result;
    QJsonObject body = document.object();

    if (!body.contains(cycle) || !body[cycle].isDouble())
    {
        Exception::ConfigCorruption exception;
        exception.error = QString("%1 is not available in the file").arg(cycle);
        throw exception;
    }
    result[Info::ExecutionPerCycle] = body[cycle].toInteger();


    if (!body.contains(queueLimit) || !body[queueLimit].isDouble())
    {
        Exception::ConfigCorruption exception;
        exception.error = QString("%1 is not available in the file").arg(queueLimit);
        throw exception;
    }
    result[Info::WaitingQueueLimit] = body[queueLimit].toInteger();


    if (!body.contains(inputLimit) || !body[inputLimit].isDouble())
    {
        Exception::ConfigCorruption exception;
        exception.error = QString("%1 is not available in the file").arg(inputLimit);
        throw exception;
    }
    result[Info::InputLimit] = body[inputLimit].toInteger();

    if (!body.contains(readyLimit) || !body[readyLimit].isDouble())
    {
        Exception::ConfigCorruption exception;
        exception.error = QString("%1 is not available in the file").arg(readyLimit);
        throw exception;
    }
    result[Info::ReadyQueueLimit] = body[readyLimit].toInteger();


    if (!body.contains(quantum) || !body[quantum].isDouble())
    {
        Exception::ConfigCorruption exception;
        exception.error = QString("%1 is not available in the file").arg(quantum);
        throw exception;
    }
    result[Info::QuantumSize] = body[quantum].toInteger();

    return result;
}

bool Config::save(const Config::Container &value)
{
    const QString fileName = Storage::workspace().absoluteFilePath(configName);
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly))
    {
        return false;
    }

    QJsonObject body;
    if (!value.contains(Info::ExecutionPerCycle))
    {
        return false;
    }
    body[cycle] = value[Info:: ExecutionPerCycle];

    if (!value.contains(Info::WaitingQueueLimit))
    {
        return false;
    }
    body[queueLimit] = value[Info::WaitingQueueLimit];

    if (!value.contains(Info::InputLimit))
    {
        return false;
    }
    body[inputLimit] = value[Info::InputLimit];

    if (!value.contains(Info::ReadyQueueLimit))
    {
        return false;
    }
    body[readyLimit] = value[Info::ReadyQueueLimit];

    if (!value.contains(Info::QuantumSize))
    {
        return false;
    }
    body[quantum] = value[Info::QuantumSize];

    QJsonDocument document(body);

    if (file.write(document.toJson()) < 0)
    {
        return false;
    }

    file.close();
    return true;
}
