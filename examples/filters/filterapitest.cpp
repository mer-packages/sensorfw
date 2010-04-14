#include <QtDebug>
#include <QTest>
#include <QVariant>

#include <typeinfo>

#include "sensord/bin.h"
#include "sensord/bufferreader.h"
#include "sensord/filter.h"
#include "sensord/dbusemitter.h"
#ifdef USE_ARIANE
#include "adaptors/arianeadaptor/arianeadaptor.h"
#endif
#include "filters/orientationoffsetter.h"
#include "filters/coordinatealignfilter/coordinatealignfilter.h"

#include "filters/orientationdata.h"
//#include "filters/coordinatealignfilter/declinationcorrectionfilter.h"

#include "filters/orientationinterpreter/orientationinterpreter.h"

#include "filterapitest.h"

////////////////////////////////////////////////////////////////////////////////////////////////// Ticker

class Ticker : public Pusher, public QThread
{
public:
    Ticker();

    void run();

    void pushNewData();

private:
    Source<unsigned> seconds_;
};

inline
Ticker::Ticker()
{
    addSource(&seconds_, "seconds");
}

inline
void Ticker::run()
{
    while (true) {
        QThread::sleep(1);
        wakeup();
    }
}

inline
void Ticker::pushNewData()
{
    static unsigned counter = 0;
    ++counter;
    seconds_.propagate(1, &counter);
}

////////////////////////////////////////////////////////////////////////////////////////////////// Modulo

class ModuloFilter : public Filter<unsigned, ModuloFilter>
{
public:
    ModuloFilter() :
        Filter<unsigned, ModuloFilter>(this, &ModuloFilter::modulo)
    {}

private:
    void modulo(unsigned n, const unsigned* values)
    {
        while (n--) {
            unsigned u = (*values++) % 10;
            source_.propagate(1, &u);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////// Numbers

class NumbersFilter : public Filter<unsigned, NumbersFilter, QString>
{
public:
    NumbersFilter() :
        Filter<unsigned, NumbersFilter, QString>(this, &NumbersFilter::number)
    {}

private:
    void number(unsigned n, const unsigned* values)
    {
        const char* const numbers[] = {
            "zero", "one",   "two",   "three", "four", "five",
            "six",  "seven", "eight", "nine",  "ten",  "over ten"
        };
        const unsigned knownNumbers = sizeof(numbers) / sizeof(numbers[0]);

        while (n--) {
            unsigned u = (*values++);
            QString s(numbers[u < knownNumbers ?
                              u : knownNumbers - 1]);
            source_.propagate(1, &s);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////// Console

class Console : public Consumer
{
public:
    Console() :
        unsigned_(this, &Console::outputUnsigned),
        string_(this, &Console::outputString),
        numSamplesReceived_(0)
    {
        addSink(&unsigned_, "unsigned");
        addSink(&string_, "string");
    }

    int numSamplesReceived() { return numSamplesReceived_; }

private:
    void outputUnsigned(unsigned n, const unsigned* values)
    {
        numSamplesReceived_ += n;
        while (n--) {
            qDebug() << "Console:" << *values++;
        }
    }

    void outputString(unsigned n, const QString* values)
    {
        numSamplesReceived_ += n;
        while (n--) {
            qDebug() << "Console:" << *values++;
        }
    }

    Sink<Console, unsigned> unsigned_;
    Sink<Console, QString>  string_;

    int numSamplesReceived_;
};

////////////////////////////////////////////////////////////////////////////////////////////////// Passthru

class PassThru : public Filter<unsigned, PassThru>
{
public:
    PassThru() :
        Filter<unsigned, PassThru>(this, &PassThru::algorithm)
    {}

private:
    void algorithm(unsigned n, const unsigned* values)
    {
        source_.propagate(n, values);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////// Shifter

class Shifter : /*public QObject,*/ public Filter<unsigned, Shifter>
{
    //Q_OBJECT

    //Q_PROPERTY(int offset READ offset WRITE setOffset)

public:
    Shifter() :
        Filter<unsigned, Shifter>(this, &Shifter::shift),
        offset_()
    {}

    int offset() { return offset_; }
    void setOffset(int o) { offset_ = o; }

private:
    void shift(unsigned n, const unsigned* values)
    {
        while (n--) {
            unsigned u = (*values++) + offset_;
            source_.propagate(1, &u);
        }
    }

    int offset_;
};

////////////////////////////////////////////////////////////////////////////////////////////////// Tests

void FilterApiTest::testBasicFilterChain()
{
    Bin                  bin1;
    Ticker               ticker;
    ModuloFilter         modulo;
    NumbersFilter        numbers;
    Console              console;

    bin1.add(&ticker,    "ticker");
    bin1.add(&modulo,    "modulo");
    bin1.add(&numbers,   "numbers");
    bin1.add(&console,   "console");

    bin1.join("ticker", "seconds",
              "modulo", "sink");
    bin1.join("modulo", "source",
              "numbers", "sink");
    bin1.join("numbers", "source",
              "console", "string");

    bin1.start();
    ticker.start();

    const int INTERVAL    = 1000;
    const int NUM_SAMPLES = 5;

    qDebug ( "Waiting %d mseconds", NUM_SAMPLES * INTERVAL );
    QTest::qWait( NUM_SAMPLES * INTERVAL + INTERVAL / 2 );

    QCOMPARE( console.numSamplesReceived(), NUM_SAMPLES );

    bin1.stop();
    ticker.terminate();
    ticker.wait();
}

void FilterApiTest::testAdvancedFilterChain()
{
    Bin                  bin1;
    Ticker               ticker;
    ModuloFilter         modulo;
    NumbersFilter        numbers;
    Console              console;
    RingBuffer<unsigned> buffer(1024);

    bin1.add(&ticker,    "ticker");
    bin1.add(&modulo,    "modulo");
    bin1.add(&numbers,   "numbers");
    bin1.add(&console,   "console");
    bin1.add(&buffer,    "buffer");

    bin1.join("ticker", "seconds",
              "modulo", "sink");
    bin1.join("modulo", "source",
              "numbers", "sink");
    bin1.join("numbers", "source",
              "console", "string");

    bin1.join("modulo", "source",
              "buffer", "sink");


    Bin      bin2;
    BufferReader<unsigned> reader(10);
    PassThru passthru;
    Shifter  shifter;
    Console  console2;

    bin2.add(&reader,   "reader");
    bin2.add(&shifter,  "shifter");
    bin2.add(&passthru, "passthru");
    bin2.add(&console2, "console2");

    bin2.join("reader", "source", "shifter", "sink");
    bin2.join("shifter", "source", "passthru", "sink");
    bin2.join("passthru", "source", "console2", "unsigned");

    shifter.setOffset(2);
    //shifter.setProperty("offset", QVariant(7));

    buffer.join(&reader);

    bin1.start();
    bin2.start();
    ticker.start();

    const int INTERVAL    = 1000;
    const int NUM_SAMPLES = 5;

    qDebug ( "Waiting %d mseconds", NUM_SAMPLES * INTERVAL );
    QTest::qWait( NUM_SAMPLES * INTERVAL + INTERVAL / 2 );

    QCOMPARE( console.numSamplesReceived(), NUM_SAMPLES );

    // TODO: terminate & join threads
    bin1.stop();
    ticker.terminate();
    ticker.wait();
    bin2.stop();
}

class TestDbusEmitter : public DbusEmitter<OrientationData> {
public:
    TestDbusEmitter() : DbusEmitter<OrientationData>(10), samplesReceived_(0) {}

    int numSamplesReceived() { return samplesReceived_; }

    void emitToDbus(const OrientationData& data)
    {
        samplesReceived_++;
        qDebug() << __PRETTY_FUNCTION__ << data.timestamp_ << data.x_ << data.y_ << data.z_;
    }

protected:
    int samplesReceived_;
};

void FilterApiTest::testMultiplexerFilterChain()
{
#ifdef USE_ARIANE
    // set up a filter chain
    Bin filterBin;
    ArianeAdaptor* ariane = dynamic_cast<ArianeAdaptor*>(ArianeAdaptor::factoryMethod("arianeadaptor"));
    Q_ASSERT( ariane );

    BufferReader<OrientationData> orientationReader(10);
    BufferReader<unsigned> offsetReader(10);

    RingBuffer<OrientationData> outputBuffer(1024);

    OrientationOffsetter orientationOffsetter;

    qDebug() << "adding";
    filterBin.add(&orientationReader, "orientation");
    filterBin.add(&offsetReader, "offset");
    filterBin.add(&orientationOffsetter, "offsetter");
    filterBin.add(&outputBuffer, "buffer");

    qDebug() << "joining";
    filterBin.join("orientation", "source", "offsetter", "orientation");
    filterBin.join("offset",      "source", "offsetter", "offset");
    filterBin.join("offsetter",   "source", "buffer",    "sink");

    qDebug() << "joining orientation";
    RingBufferBase* rb = ariane->findBuffer("orientation");
    Q_ASSERT(rb);
    rb->join(&orientationReader);

    qDebug() << "joining unsigned";
    rb = ariane->findBuffer("unsigned");
    Q_ASSERT(rb);
    rb->join(&offsetReader);

    qDebug() << "creating fake marshaller";
    TestDbusEmitter dbusEmitter;
    Bin marshallingBin;
    marshallingBin.add(&dbusEmitter, "testdbusemitter");

    qDebug() << "joining this";
    outputBuffer.join(&dbusEmitter);

    qDebug() << "starting";
    marshallingBin.start();
    filterBin.start();
    ariane->startAdaptor();

    ariane->startSensor("orientation");
    ariane->startSensor("unsigned");

    const int INTERVAL    = 1000;
    const int NUM_SAMPLES = 5;

    qDebug ( "Waiting %d mseconds", NUM_SAMPLES * INTERVAL );
    QTest::qWait( NUM_SAMPLES * INTERVAL + INTERVAL / 2 );

    QCOMPARE( dbusEmitter.numSamplesReceived(), NUM_SAMPLES );

    qDebug() << "ariane_->stop()";
    ariane->stopAdaptor();

    qDebug() << "filterBin_->stop()";
    filterBin.stop();
    marshallingBin.stop();

    delete ariane;
#endif
}

#if 0
/**
 * DummyAdaptor is a Pusher that can be used to push data into a filter for testing.
 * Input data is given as an array. Calling \c pushNewData() will propagate the next
 * value in the array into adaptor output.
 */
template <class TYPE>
class DummyAdaptor : public Pusher
{
public:
    DummyAdaptor() : counter_(0), datacount_(0), data_(NULL), loop_(false), index_(0) {
        addSource(&source_, "source");
    };

    void setTestData(int count, TYPE *data, bool loop) {
        datacount_ = count;
        loop_ = loop;
        data_ = data;
        index_ = 0;
    }
    
    void pushNewData() {
        if (index_ >= datacount_) {
            QVERIFY2(loop_, "Test function error: out of input data.");
            index_ = 0;
        }

        source_.propagate(1, &(data_[index_]));

        index_++;
        counter_++;
    }
       
    int getDataCount() { return counter_; }

private:
    Source<TYPE> source_;
    int counter_;
    int datacount_;
    TYPE *data_;
    bool loop_;
    int index_;
};



/**
 * DummyDbusEmitter is a DbusEmitter that can be used to capture output data of
 * a filter for testing purposes. The expected output data is given through a 
 * function, and is compared to actual received output  in \c emitToDbus() function.
 */
template <class TYPE>
class DummyDbusEmitter : public DbusEmitter<TYPE> {
public:
    DummyDbusEmitter() : DbusEmitter<TYPE>(10), 
                         samplesReceived_(0),
                         datacount_(0),
                         data_(NULL),
                         loop_(false),
                         index_(0) {}

    void setExpectedData(int count, TYPE *data, bool loop) {
        datacount_ = count;
        data_ = data;
        loop_ = loop;
        index_ = 0;
    }
    int numSamplesReceived() { return samplesReceived_; }

    void emitToDbus(const TYPE& data) {
        index_++;
        samplesReceived_++;

        if (index_ > datacount_) {
            QVERIFY2(loop_, "Test function error: out of expected results.");
            index_ = 0;
        }

        // Check type
        if (typeid(TYPE) == typeid(PoseData)) {
            PoseData *d1 = (PoseData *)&data;
            PoseData *d2 = (PoseData *)&(data_[index_-1]);
            QCOMPARE(d1->orientation_, d2->orientation_);

        } else if (typeid(TYPE) == typeid(TimedXyzData)) {
            TimedXyzData *d1 = (TimedXyzData *)&data;
            TimedXyzData *d2 = (TimedXyzData *)&(data_[index_-1]);

            QCOMPARE(d1->timestamp_, d2->timestamp_);
            QCOMPARE(d1->x_, d2->x_);
            QCOMPARE(d1->y_, d2->y_);
            QCOMPARE(d1->z_, d2->z_);

        } else {
            /*
             TYPE *d1 = &data;
             TYPE *d2 = &data_[index_-1];
             QCOMPARE(*d1, *d2);
             */
            //TODO: Figure out how to compare types that have '==' operator so that
            //      compiler will accept it.
            QWARN("No comparison method for this type");
        }
    }

protected:
    int samplesReceived_;
    int datacount_;
    TYPE *data_;
    bool loop_;
    int index_;
};
#endif

QTEST_MAIN(FilterApiTest)
