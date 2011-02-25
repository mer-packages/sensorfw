#include "parser.h"
#include <QDebug>
#include <iostream>

Parser::Parser(QStringList arguments)
{
    sensors << "orientationsensor" << "accelerometersensor"
            << "compasssensor" << "tapsensor" << "alssensor"
            << "proximitysensor" << "rotationsensor" << "magnetometersensor" ;
    parsingCommandLine(arguments);
}

void Parser::parsingCommandLine(QStringList args)
{
    if (args.size() == 1){
        qDebug() << "Usage: sensorthread-test sensor1 num1 sensor2 num2 ....";
        qDebug() <<  "sensors:";
        qDebug() << "orientationsensor, " << "accelerometersensor";
        qDebug()  << "compasssensor, " << "tapsensor, " << "alssensor";
        qDebug()  << "proximitysensor, " << "rotationsensor ," << "magnetometersensor" ;

        return;
    }

    QString sensorName = NULL;

    args.removeFirst();

    int i = 0;
    int numberOfThread;

    foreach (const QString& arg, args)
    {
        switch(i)
        {
        case 0:
            sensorName = arg.trimmed();
            if (sensors.contains(sensorName)){
                i++;
            } else {
                qDebug() << sensorName << " doesn't exist, skip it";
            }
            break;
        case 1:
            numberOfThread = arg.trimmed().toInt();
            qDebug() << sensorName << ": " << numberOfThread << " threads to run";
            sensorThread.insert(sensorName, numberOfThread);
            i = 0;
            break;
         }
    }
}

QMap<QString, int> Parser:: getSensorThread()
{
    return sensorThread;
}
