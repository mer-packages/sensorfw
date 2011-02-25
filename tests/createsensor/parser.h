#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include "logging.h"

class Parser
{
public:

    Parser(QStringList arguments);

    QMap<QString, int> getSensorThread();

    void parsingCommandLine(QStringList args);

    bool changeLogLevel() const;
    SensordLogLevel getLogLevel() const;

    bool configFileInput() const;
    const QString& configFilePath() const;

    int logTarget() const;
    const QString& logFilePath() const;

private:
    QMap<QString, int> sensorThread;
    QStringList sensors;

    bool configFile_;
    bool changeLogLevel_;

    QString configFilePath_;
    SensordLogLevel logLevel_;
    int logTarget_; //TODO: add some enum about log targets
    QString logFilePath_;
};

#endif // PARSER_H
