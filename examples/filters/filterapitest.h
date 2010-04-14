#ifndef FILTERAPITEST_H
#define FILTERAPITEST_H

#include <QTest>

class FilterApiTest : public QObject
{
     Q_OBJECT

private slots:
    /*
    void initTestCase();
     { qDebug("called before any testfunction is executed"); }
    void init()
     { qDebug("called before each testfunction is executed"); }
    */

    void testBasicFilterChain();
    void testAdvancedFilterChain();
    void testMultiplexerFilterChain();

    /*
    void cleanup()
     { qDebug("called after every testfunction"); }
    void cleanupTestCase()
     { qDebug("called after all tests have finished"); }
     */
};

#endif // FILTERAPITEST_H
