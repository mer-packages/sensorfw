#ifndef TESTCLASS_H
#define TESTCLASS_H

#include <QtDebug>
#include <QObject>
#include <QDBusVariant>
#include <QDBusArgument>

#include "datatypes/orientation.h"
#include "datatypes/xyz.h"
#include "datatypes/unsigned.h"

#ifdef USE_MOTION_BAND
#include "datatypes/raworientation.h"
#endif

#ifdef USE_ARIANE
#include "datatypes/key.h"
#endif

#ifdef USE_INTERNAL
#include "datatypes/tap.h"
#include "filters/posedata.h"
#endif

#include "sensord/sensormanager.h"

class TestErrorSignalReceiver : public QObject {
    Q_OBJECT

public:
    TestErrorSignalReceiver() : numErrorSignalsReceived_(0), errorCode_(SmNoError) {}

    int numErrorSignalsReceived_;

public Q_SLOTS:
#if 1
    void errorSlot(int errorCode)
    {
        //qDebug() << __PRETTY_FUNCTION__ << errorCode;

        errorCode_ = static_cast<SensorManagerError>(errorCode);
        numErrorSignalsReceived_++;
    }
#else
    /*void errorSlot(SensorManagerError errorCode)
    {
        qDebug() << __PRETTY_FUNCTION__ << errorCode;

        errorCode_ = errorCode;
        numErrorSignalsReceived_++;
    }*/
#endif
    SensorManagerError errorCode() { return errorCode_; errorCode_ = SmNoError; }

private:
    SensorManagerError errorCode_;
};

class TestClass : public QObject {
    Q_OBJECT

public:
    TestClass(const QString& name)
        : numStateChanges(0)
        , numSamplesReceived(0)
        , prevCompassDegrees(0)
        , prevCompassLevel(0)
        , prevALS(0)
        , name_(name) {}
    TestClass(QObject* parent) : QObject(parent) {}
    virtual ~TestClass() {}

    int numStateChanges;
    volatile int numSamplesReceived;

    int prevCompassDegrees;
    int prevCompassLevel;
#ifdef USE_INTERNAL
    PoseData prevOrientation;
#endif

    int prevALS;
	
    QString  lastPropertyChangedName;
    QVariant lastPropertyChangedValue;

public slots:
    void handleData(const Orientation& data)
    {
        numSamplesReceived++;
        qDebug() << name_ << "Orientation SensorData ="
                 << data.orientationData().timestamp_
                 << data.orientationData().x_
                 << data.orientationData().y_
                 << data.orientationData().z_;
    }

    void handleData(const QVariant& data)
    {
        numSamplesReceived++;
        qDebug() << name_ << "QVariant SensorData ="
                 << data.typeName()
                 << data;
    }

    void handleData(const QDBusVariant& data)
    {
        numSamplesReceived++;
        qDebug() << name_ << "QDBusVariant SensorData ="
                 << data.variant().typeName()
                 << data.variant();

        if ( data.variant().canConvert<QDBusArgument>() )
        {
            qDebug() << name_ << "QDBusArgument ElementType =" 
                     << data.variant().value<QDBusArgument>().currentType();
        }
    }

#ifdef USE_MOTION_BAND
    void handleData(const RawOrientation& data)
    {
        numSamplesReceived++;
        qDebug() << data.deviceAxesData().x.x
                 << data.deviceAxesData().x.y
                 << data.deviceAxesData().y.z
                 << "...";
    }
#endif

    void handleCompassLevel(const int& level)
    {
            qDebug() << "[Compass]: level   " << level;
            prevCompassLevel = level;
    }

    void handleCompassDegrees(const int& degrees)
    {
            qDebug() << "[Compass]: degrees " << degrees;
            prevCompassDegrees = degrees;
    }

#ifdef USE_INTERNAL
    void handleOrientationChange(const int& orientation)
    {
            // TODO: cast and print out enums
            prevOrientation.orientation_ = (PoseData::Orientation)orientation;
            qDebug() << "[Orientation]:" << prevOrientation.orientation_;
    }
#endif

    void handleData(const XYZ& data)
    {
        numSamplesReceived++;
        /*
        qDebug() << name_ << "XYZ SensorData ="
                 << data.XYZData().timestamp_
                 << data.XYZData().x_
                 << data.XYZData().y_
                 << data.XYZData().z_;*/
    }

    void handleProximity(const int& state)
    {
        qDebug() << "Proximity state: " << state;
    }
    
    void handleData(const Unsigned& data)
    {
        numSamplesReceived++;
        qDebug() << name_ << "Unsigned SensorData ="
                 << data.UnsignedData().timestamp_
                 << data.UnsignedData().value_;
    }
#ifdef USE_ARIANE
    void handleData(const Key& data)
    {
        numSamplesReceived++;
        qDebug() << name_ << "Key SensorData ="
                 << data.KeyData().timestamp_
                 << (char)data.KeyData().value_.key << "(" << data.KeyData().value_.key << ")"
                 << data.KeyData().value_.pressed
                 << data.KeyData().value_.released;
    }
#endif
    void handleALSChanged(const int& value) {
        qDebug() << "[ALS]: " << value;
        prevALS = value;
    }

#ifdef USE_INTERNAL
    void handleTapEvent(const Tap& tap) {
        qDebug() << "[Tap]: direction: " << tap.tapData().direction_
            << " type: " << (tap.tapData().type_ ? "Single" : "Double");
    }
#endif

    void handlePropertyChange(const QString& name)
    {
        QObject* sender = this->sender();
        Q_ASSERT( sender );

        QVariant newValue = sender->property(name.toAscii().constData());
        qDebug() << "Property" << name << "changed to" << newValue;

        lastPropertyChangedName  = name;
        lastPropertyChangedValue = newValue;
    }

private:
    QString name_;
};

#endif // TESTCLASS_H
