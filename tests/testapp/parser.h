#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include "logging.h"

class Parser
{
public:

    Parser(QStringList arguments);

    void parsingCommandLine(QStringList args);

    bool printHelp() const;

    bool changeLogLevel() const;
    SensordLogLevel getLogLevel() const;

    bool configFileInput() const;
    const QString& configFilePath() const;

    bool singleThread() const;
    bool gracefulShutdown() const;

    int logTarget() const;
    const QString& logFilePath() const;

    int statInterval() const;

private:
    bool printHelp_;
    bool changeLogLevel_;
    bool configFile_;
    bool singleThread_;
    bool gracefulShutdown_;

    QString configFilePath_;
    SensordLogLevel logLevel_;
    int logTarget_;
    QString logFilePath_;
    int statInterval_;
};

#endif // PARSER_H
