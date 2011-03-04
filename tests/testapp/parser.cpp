#include "parser.h"
#include <QDebug>
#include <iostream>

Parser::Parser()
{

}

Parser::Parser(QStringList arguments) :
    printHelp_(false),
    changeLogLevel_(false),
    configFile_(false),
    singleThread_(true),
    gracefulShutdown_(true),
    configFilePath_(""),
    logLevel_(SensordLogWarning),
    logTarget_(1),
    logFilePath_("/var/log/sensors-testapp.log"),
    statInterval_(5000)
{
    parsingCommandLine(arguments);
}

void Parser::parsingCommandLine(QStringList arguments)
{
    foreach (const QString& arg, arguments)
    {
        QStringList data;
        QString opt = arg.trimmed();

        if (opt.startsWith("-l=") || opt.startsWith("--log-level"))
        {
            data = opt.split("=");
            changeLogLevel_ = true;
            QString logLevel = data.at(1);
            if (logLevel == "test")
                logLevel_ = SensordLogTest;
            else if (logLevel == "debug")
                logLevel_ = SensordLogDebug;
            else if (logLevel == "warning")
                logLevel_ = SensordLogWarning;
            else if (logLevel == "critical")
                logLevel_ = SensordLogCritical;
            else
                logLevel_ = SensordLogWarning;
        }
        else if (opt.startsWith("-i=") || opt.startsWith("--stat-interval"))
        {
            data = opt.split("=");
            statInterval_ = data.at(1).toInt();
        }
        else if (opt.startsWith("--log-target"))
        {
            data = opt.split("=");
            logTarget_= data.at(1).toInt();
        }
        else if (opt.startsWith("--log-file-path"))
        {
            data = opt.split("=");
            logFilePath_ = data.at(1);
        }
        else if (opt.startsWith("-c=") || opt.startsWith("--config-file"))
        {
            data = opt.split("=");
            configFile_ = true;
            configFilePath_ = data.at(1);
        }
        else if (opt.startsWith("-m="))
        {
            data = opt.split("=");
            if (data.at(1).toInt() == 2)
            {
                singleThread_ = false ;
            }
        }
        else if (opt.startsWith("-g="))
        {
            data = opt.split("=");
            gracefulShutdown_ = !data.at(1).toInt() == 0;
        }
        else if (opt.startsWith("-h") || opt.startsWith("--help"))
            printHelp_ = true;
        else if (opt.startsWith("-"))
            std::cerr << "Unknown option: " << opt.toStdString() << std::endl;
    }
}

bool Parser::changeLogLevel() const
{
    return changeLogLevel_;
}

SensordLogLevel Parser::getLogLevel() const
{
    return logLevel_;
}

bool Parser::configFileInput() const
{
    return configFile_;
}

const QString& Parser::configFilePath() const
{
    return configFilePath_;
}

bool Parser::singleThread() const
{
    return singleThread_;
}

int Parser::logTarget() const
{
    return logTarget_;
}

const QString& Parser::logFilePath() const
{
    return logFilePath_;
}

bool Parser::printHelp() const
{
    return printHelp_;
}

int Parser::statInterval() const
{
    return statInterval_;
}

bool Parser::gracefulShutdown() const
{
    return gracefulShutdown_;
}
