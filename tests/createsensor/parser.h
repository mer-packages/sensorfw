#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QStringList>
#include <QMap>

class Parser
{
public:

    Parser(QStringList arguments);

    QMap<QString, int> getSensorThread();

    void parsingCommandLine(QStringList args);

private:
    QMap<QString, int> sensorThread;
    QStringList sensors;

};

#endif // PARSER_H
