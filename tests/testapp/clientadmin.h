#ifndef CLIENTADMIN_H
#define CLIENTADMIN_H

#include <QObject>

#include <QTest>
#include <QList>
#include <QString>
#include <QDebug>
#include <signal.h>

#include "sensorhandler.h"
#include "parser.h"
#include "config.h"
#include "logging.h"
#include "statprinter.h"

class ClientAdmin : public QObject
{
    Q_OBJECT
public:
    ClientAdmin(const Parser& parser, QObject *parent = 0);
    ~ClientAdmin();
    void runClients();

signals:

public slots:

private:
    Parser parser;
    QList<SensorHandler*> handlers;
    StatPrinter* printer;
    static const char* CONFIG_FILE_PATH;

    void init();
    static void registerSensorInterface(const QStringList& sensors);
};

#endif // CLIENTADMIN_H
