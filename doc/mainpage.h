/**

@mainpage Sensor Framework API documentation

@section introsection Introduction

Sensor Framework provides access to hardware sensors and additional
logical products calculated from them.

<hr>

@section devinfosection Sensorfw Developer Info

@ref developer

@ref dbusinterface

<a href="classes.html">Class Index</a>

<a href="annotated.html">Class List</a>

@subsection apisubsection Public API

- <a href="classSensorManagerInterface.html">SensorManagerInterface</a>

- <a href="classLocalSensorManagerInterface.html">LocalSensorManagerInterface</a>

- <a href="classAbstractSensorChannelInterface.html">AbstractSensorChannelInterface</a>
- <a href="classAccelerometerSensorChannelInterface.html">AccelerometerSensorChannelInterface</a>
- <a href="classALSSensorChannelInterface.html">ALSSensorChannelInterface</a>
- <a href="classCompassSensorChannelInterface.html">CompassSensorChannelInterface</a>
- <a href="classGyroscopeSensorInterface.html">GyroscopeSensorChannelInterface</a>
- <a href="classMagnetometerSensorChannelInterface.html">MagnetometerSensorChannelInterface</a>
- <a href="classOrientationSensorChannelInterface.html">OrientationSensorChannelInterface</a>
- <a href="classProximitySensorChannelInterface.html">ProximitySensorChannelInterface</a>
- <a href="classRotationSensorChannelInterface.html">RotationSensorChannelInterface</a>
- <a href="classTapSensorChannelInterface.html">TapSensorChannelInterface</a>
- <a href="classSensorManagerInterface.html">SensorManagerInterface</a>

@page dbusinterface Dbus Interface

Sensorfw has a dbus interface for control

@code
qdbus --system com.nokia.SensorService

/

qdbus --system com.nokia.SensorService /
method QString org.freedesktop.DBus.Introspectable.Introspect()
method QString org.freedesktop.DBus.Peer.GetMachineId()
method void org.freedesktop.DBus.Peer.Ping()

/SensorManager

qdbus --system com.nokia.SensorService /SensorManager
property read int local.SensorManager.errorCodeInt
property read QString local.SensorManager.errorString
property readwrite int local.SensorManager.magneticDeviation
signal void local.SensorManager.errorSignal(int error)
method bool local.SensorManager.loadPlugin(QString name)
method double local.SensorManager.magneticDeviation()
method bool local.SensorManager.releaseSensor(QString id, int sessionId, qlonglong pid)
method int local.SensorManager.requestSensor(QString id, qlonglong pid)
method void local.SensorManager.setMagneticDeviation(double level)
method QDBusVariant org.freedesktop.DBus.Properties.Get(QString interface_name, QString property_name)
method QVariantMap org.freedesktop.DBus.Properties.GetAll(QString interface_name)
method void org.freedesktop.DBus.Properties.Set(QString interface_name, QString property_name, QDBusVariant value)
method QString org.freedesktop.DBus.Introspectable.Introspect()
method QString org.freedesktop.DBus.Peer.GetMachineId()
method void org.freedesktop.DBus.Peer.Ping()

/SensorManager/accelerometersensor

qdbus --system com.nokia.SensorService /SensorManager/accelerometersensor
property read uint local.AccelerometerSensor.bufferInterval
property read uint local.AccelerometerSensor.bufferSize
property read QString local.AccelerometerSensor.description
property read int local.AccelerometerSensor.errorCodeInt
property read QString local.AccelerometerSensor.errorString
property read bool local.AccelerometerSensor.hwBuffering
property read QString local.AccelerometerSensor.id
property read uint local.AccelerometerSensor.interval
property read bool local.AccelerometerSensor.isValid
property read bool local.AccelerometerSensor.standbyOverride
property read QString local.AccelerometerSensor.type
property read QDBusRawType::(tiii) local.AccelerometerSensor.xyz
signal void local.AccelerometerSensor.dataAvailable(QDBusRawType::(tiii) data)
signal void local.AccelerometerSensor.propertyChanged(QString name)
method uint local.AccelerometerSensor.bufferInterval()
method uint local.AccelerometerSensor.bufferSize()
method QString local.AccelerometerSensor.description()
method QString local.AccelerometerSensor.errorString()
method QDBusRawType::a(uu) local.AccelerometerSensor.getAvailableBufferIntervals()
method QDBusRawType::a(uu) local.AccelerometerSensor.getAvailableBufferSizes()
method QDBusRawType::a(ddd) local.AccelerometerSensor.getAvailableDataRanges()
method QDBusRawType::a(ddd) local.AccelerometerSensor.getAvailableIntervals()
method QDBusRawType::(ddd) local.AccelerometerSensor.getCurrentDataRange()
method bool local.AccelerometerSensor.hwBuffering()
method QString local.AccelerometerSensor.id()
method uint local.AccelerometerSensor.interval()
method bool local.AccelerometerSensor.isValid()
method void local.AccelerometerSensor.removeDataRangeRequest(int sessionId)
method void local.AccelerometerSensor.requestDataRange(int sessionId, QDBusRawType::(ddd) range)
method void local.AccelerometerSensor.setBufferInterval(int sessionId, uint value)
method void local.AccelerometerSensor.setBufferSize(int sessionId, uint value)
method bool local.AccelerometerSensor.setDataRangeIndex(int sessionId, int rangeIndex)
method bool local.AccelerometerSensor.setDefaultInterval(int sessionId)
method void local.AccelerometerSensor.setDownsampling(int sessionId, bool value)
method void local.AccelerometerSensor.setInterval(int sessionId, int value)
method bool local.AccelerometerSensor.setStandbyOverride(int sessionId, bool value)
method bool local.AccelerometerSensor.standbyOverride()
method void local.AccelerometerSensor.start(int sessionId)
method void local.AccelerometerSensor.stop(int sessionId)
method QString local.AccelerometerSensor.type()
method QDBusRawType::(tiii) local.AccelerometerSensor.xyz()
method QDBusVariant org.freedesktop.DBus.Properties.Get(QString interface_name, QString property_name)
method QVariantMap org.freedesktop.DBus.Properties.GetAll(QString interface_name)
method void org.freedesktop.DBus.Properties.Set(QString interface_name, QString property_name, QDBusVariant value)
method QString org.freedesktop.DBus.Introspectable.Introspect()
method QString org.freedesktop.DBus.Peer.GetMachineId()
method void org.freedesktop.DBus.Peer.Ping()

/SensorManager/alssensor

qdbus --system com.nokia.SensorService /SensorManager/alssensor
property read uint local.ALSSensor.bufferInterval
property read uint local.ALSSensor.bufferSize
property read QString local.ALSSensor.description
property read int local.ALSSensor.errorCodeInt
property read QString local.ALSSensor.errorString
property read bool local.ALSSensor.hwBuffering
property read QString local.ALSSensor.id
property read uint local.ALSSensor.interval
property read bool local.ALSSensor.isValid
property read QDBusRawType::(tu) local.ALSSensor.lux
property read bool local.ALSSensor.standbyOverride
property read QString local.ALSSensor.type
signal void local.ALSSensor.ALSChanged(QDBusRawType::(tu) value)
signal void local.ALSSensor.propertyChanged(QString name)
method uint local.ALSSensor.bufferInterval()
method uint local.ALSSensor.bufferSize()
method QString local.ALSSensor.description()
method QString local.ALSSensor.errorString()
method QDBusRawType::a(uu) local.ALSSensor.getAvailableBufferIntervals()
method QDBusRawType::a(uu) local.ALSSensor.getAvailableBufferSizes()
method QDBusRawType::a(ddd) local.ALSSensor.getAvailableDataRanges()
method QDBusRawType::a(ddd) local.ALSSensor.getAvailableIntervals()
method QDBusRawType::(ddd) local.ALSSensor.getCurrentDataRange()
method bool local.ALSSensor.hwBuffering()
method QString local.ALSSensor.id()
method uint local.ALSSensor.interval()
method bool local.ALSSensor.isValid()
method QDBusRawType::(tu) local.ALSSensor.lux()
method void local.ALSSensor.removeDataRangeRequest(int sessionId)
method void local.ALSSensor.requestDataRange(int sessionId, QDBusRawType::(ddd) range)
method void local.ALSSensor.setBufferInterval(int sessionId, uint value)
method void local.ALSSensor.setBufferSize(int sessionId, uint value)
method bool local.ALSSensor.setDataRangeIndex(int sessionId, int rangeIndex)
method bool local.ALSSensor.setDefaultInterval(int sessionId)
method void local.ALSSensor.setDownsampling(int sessionId, bool value)
method void local.ALSSensor.setInterval(int sessionId, int value)
method bool local.ALSSensor.setStandbyOverride(int sessionId, bool value)
method bool local.ALSSensor.standbyOverride()
method void local.ALSSensor.start(int sessionId)
method void local.ALSSensor.stop(int sessionId)
method QString local.ALSSensor.type()
method QDBusVariant org.freedesktop.DBus.Properties.Get(QString interface_name, QString property_name)
method QVariantMap org.freedesktop.DBus.Properties.GetAll(QString interface_name)
method void org.freedesktop.DBus.Properties.Set(QString interface_name, QString property_name, QDBusVariant value)
method QString org.freedesktop.DBus.Introspectable.Introspect()
method QString org.freedesktop.DBus.Peer.GetMachineId()
method void org.freedesktop.DBus.Peer.Ping()

/SensorManager/gyroscopesensor

qdbus --system com.nokia.SensorService /SensorManager/gyroscopesensor
property read uint local.GyroscopeSensor.bufferInterval
property read uint local.GyroscopeSensor.bufferSize
property read QString local.GyroscopeSensor.description
property read int local.GyroscopeSensor.errorCodeInt
property read QString local.GyroscopeSensor.errorString
property read bool local.GyroscopeSensor.hwBuffering
property read QString local.GyroscopeSensor.id
property read uint local.GyroscopeSensor.interval
property read bool local.GyroscopeSensor.isValid
property read bool local.GyroscopeSensor.standbyOverride
property read QString local.GyroscopeSensor.type
property read QDBusRawType::(tiii) local.GyroscopeSensor.value
signal void local.GyroscopeSensor.dataAvailable(QDBusRawType::(tiii) data)
signal void local.GyroscopeSensor.propertyChanged(QString name)
method uint local.GyroscopeSensor.bufferInterval()
method uint local.GyroscopeSensor.bufferSize()
method QString local.GyroscopeSensor.description()
method QString local.GyroscopeSensor.errorString()
method QDBusRawType::a(uu) local.GyroscopeSensor.getAvailableBufferIntervals()
method QDBusRawType::a(uu) local.GyroscopeSensor.getAvailableBufferSizes()
method QDBusRawType::a(ddd) local.GyroscopeSensor.getAvailableDataRanges()
method QDBusRawType::a(ddd) local.GyroscopeSensor.getAvailableIntervals()
method QDBusRawType::(ddd) local.GyroscopeSensor.getCurrentDataRange()
method bool local.GyroscopeSensor.hwBuffering()
method QString local.GyroscopeSensor.id()
method uint local.GyroscopeSensor.interval()
method bool local.GyroscopeSensor.isValid()
method void local.GyroscopeSensor.removeDataRangeRequest(int sessionId)
method void local.GyroscopeSensor.requestDataRange(int sessionId, QDBusRawType::(ddd) range)
method void local.GyroscopeSensor.setBufferInterval(int sessionId, uint value)
method void local.GyroscopeSensor.setBufferSize(int sessionId, uint value)
method bool local.GyroscopeSensor.setDataRangeIndex(int sessionId, int rangeIndex)
method bool local.GyroscopeSensor.setDefaultInterval(int sessionId)
method void local.GyroscopeSensor.setDownsampling(int sessionId, bool value)
method void local.GyroscopeSensor.setInterval(int sessionId, int value)
method bool local.GyroscopeSensor.setStandbyOverride(int sessionId, bool value)
method bool local.GyroscopeSensor.standbyOverride()
method void local.GyroscopeSensor.start(int sessionId)
method void local.GyroscopeSensor.stop(int sessionId)
method QString local.GyroscopeSensor.type()
method QDBusRawType::(tiii) local.GyroscopeSensor.value()
method QDBusVariant org.freedesktop.DBus.Properties.Get(QString interface_name, QString property_name)
method QVariantMap org.freedesktop.DBus.Properties.GetAll(QString interface_name)
method void org.freedesktop.DBus.Properties.Set(QString interface_name, QString property_name, QDBusVariant value)
method QString org.freedesktop.DBus.Introspectable.Introspect()
method QString org.freedesktop.DBus.Peer.GetMachineId()
method void org.freedesktop.DBus.Peer.Ping()

/SensorManager/magnetometersensor

qdbus --system com.nokia.SensorService /SensorManager/magnetometersensor
property read uint local.MagnetometerSensor.bufferInterval
property read uint local.MagnetometerSensor.bufferSize
property read QString local.MagnetometerSensor.description
property read int local.MagnetometerSensor.errorCodeInt
property read QString local.MagnetometerSensor.errorString
property read bool local.MagnetometerSensor.hwBuffering
property read QString local.MagnetometerSensor.id
property read uint local.MagnetometerSensor.interval
property read bool local.MagnetometerSensor.isValid
property read QDBusRawType::(tiiiiiii) local.MagnetometerSensor.magneticField
property read bool local.MagnetometerSensor.standbyOverride
property read QString local.MagnetometerSensor.type
signal void local.MagnetometerSensor.dataAvailable(QDBusRawType::(tiiiiiii) data)
signal void local.MagnetometerSensor.propertyChanged(QString name)
method uint local.MagnetometerSensor.bufferInterval()
method uint local.MagnetometerSensor.bufferSize()
method QString local.MagnetometerSensor.description()
method QString local.MagnetometerSensor.errorString()
method QDBusRawType::a(uu) local.MagnetometerSensor.getAvailableBufferIntervals()
method QDBusRawType::a(uu) local.MagnetometerSensor.getAvailableBufferSizes()
method QDBusRawType::a(ddd) local.MagnetometerSensor.getAvailableDataRanges()
method QDBusRawType::a(ddd) local.MagnetometerSensor.getAvailableIntervals()
method QDBusRawType::(ddd) local.MagnetometerSensor.getCurrentDataRange()
method bool local.MagnetometerSensor.hwBuffering()
method QString local.MagnetometerSensor.id()
method uint local.MagnetometerSensor.interval()
method bool local.MagnetometerSensor.isValid()
method QDBusRawType::(tiiiiiii) local.MagnetometerSensor.magneticField()
method void local.MagnetometerSensor.removeDataRangeRequest(int sessionId)
method void local.MagnetometerSensor.requestDataRange(int sessionId, QDBusRawType::(ddd) range)
method void local.MagnetometerSensor.reset()
method void local.MagnetometerSensor.setBufferInterval(int sessionId, uint value)
method void local.MagnetometerSensor.setBufferSize(int sessionId, uint value)
method bool local.MagnetometerSensor.setDataRangeIndex(int sessionId, int rangeIndex)
method bool local.MagnetometerSensor.setDefaultInterval(int sessionId)
method void local.MagnetometerSensor.setDownsampling(int sessionId, bool value)
method void local.MagnetometerSensor.setInterval(int sessionId, int value)
method bool local.MagnetometerSensor.setStandbyOverride(int sessionId, bool value)
method bool local.MagnetometerSensor.standbyOverride()
method void local.MagnetometerSensor.start(int sessionId)
method void local.MagnetometerSensor.stop(int sessionId)
method QString local.MagnetometerSensor.type()
method QDBusVariant org.freedesktop.DBus.Properties.Get(QString interface_name, QString property_name)
method QVariantMap org.freedesktop.DBus.Properties.GetAll(QString interface_name)
method void org.freedesktop.DBus.Properties.Set(QString interface_name, QString property_name, QDBusVariant value)
method QString org.freedesktop.DBus.Introspectable.Introspect()
method QString org.freedesktop.DBus.Peer.GetMachineId()
method void org.freedesktop.DBus.Peer.Ping()

/SensorManager/orientationsensor

qdbus --system com.nokia.SensorService /SensorManager/orientationsensor
property read uint local.OrientationSensor.bufferInterval
property read uint local.OrientationSensor.bufferSize
property read QString local.OrientationSensor.description
property read int local.OrientationSensor.errorCodeInt
property read QString local.OrientationSensor.errorString
property read bool local.OrientationSensor.hwBuffering
property read QString local.OrientationSensor.id
property read uint local.OrientationSensor.interval
property read bool local.OrientationSensor.isValid
property read QDBusRawType::(tu) local.OrientationSensor.orientation
property read bool local.OrientationSensor.standbyOverride
property readwrite int local.OrientationSensor.threshold
property read QString local.OrientationSensor.type
signal void local.OrientationSensor.orientationChanged(QDBusRawType::(tu) orientation)
signal void local.OrientationSensor.propertyChanged(QString name)
method uint local.OrientationSensor.bufferInterval()
method uint local.OrientationSensor.bufferSize()
method QString local.OrientationSensor.description()
method QString local.OrientationSensor.errorString()
method QDBusRawType::a(uu) local.OrientationSensor.getAvailableBufferIntervals()
method QDBusRawType::a(uu) local.OrientationSensor.getAvailableBufferSizes()
method QDBusRawType::a(ddd) local.OrientationSensor.getAvailableDataRanges()
method QDBusRawType::a(ddd) local.OrientationSensor.getAvailableIntervals()
method QDBusRawType::(ddd) local.OrientationSensor.getCurrentDataRange()
method bool local.OrientationSensor.hwBuffering()
method QString local.OrientationSensor.id()
method uint local.OrientationSensor.interval()
method bool local.OrientationSensor.isValid()
method QDBusRawType::(tu) local.OrientationSensor.orientation()
method void local.OrientationSensor.removeDataRangeRequest(int sessionId)
method void local.OrientationSensor.requestDataRange(int sessionId, QDBusRawType::(ddd) range)
method void local.OrientationSensor.setBufferInterval(int sessionId, uint value)
method void local.OrientationSensor.setBufferSize(int sessionId, uint value)
method bool local.OrientationSensor.setDataRangeIndex(int sessionId, int rangeIndex)
method bool local.OrientationSensor.setDefaultInterval(int sessionId)
method void local.OrientationSensor.setDownsampling(int sessionId, bool value)
method void local.OrientationSensor.setInterval(int sessionId, int value)
method bool local.OrientationSensor.setStandbyOverride(int sessionId, bool value)
method void local.OrientationSensor.setThreshold(int value)
method bool local.OrientationSensor.standbyOverride()
method void local.OrientationSensor.start(int sessionId)
method void local.OrientationSensor.stop(int sessionId)
method int local.OrientationSensor.threshold()
method QString local.OrientationSensor.type()
method QDBusVariant org.freedesktop.DBus.Properties.Get(QString interface_name, QString property_name)
method QVariantMap org.freedesktop.DBus.Properties.GetAll(QString interface_name)
method void org.freedesktop.DBus.Properties.Set(QString interface_name, QString property_name, QDBusVariant value)
method QString org.freedesktop.DBus.Introspectable.Introspect()
method QString org.freedesktop.DBus.Peer.GetMachineId()
method void org.freedesktop.DBus.Peer.Ping()

/SensorManager/proximitysensor

qdbus --system com.nokia.SensorService /SensorManager/proximitysensor
property read uint local.ProximitySensor.bufferInterval
property read uint local.ProximitySensor.bufferSize
property read QString local.ProximitySensor.description
property read int local.ProximitySensor.errorCodeInt
property read QString local.ProximitySensor.errorString
property read bool local.ProximitySensor.hwBuffering
property read QString local.ProximitySensor.id
property read uint local.ProximitySensor.interval
property read bool local.ProximitySensor.isValid
property read QDBusRawType::(tu) local.ProximitySensor.proximity
property read bool local.ProximitySensor.standbyOverride
property read QString local.ProximitySensor.type
signal void local.ProximitySensor.dataAvailable(QDBusRawType::(tu) data)
signal void local.ProximitySensor.propertyChanged(QString name)
method uint local.ProximitySensor.bufferInterval()
method uint local.ProximitySensor.bufferSize()
method QString local.ProximitySensor.description()
method QString local.ProximitySensor.errorString()
method QDBusRawType::a(uu) local.ProximitySensor.getAvailableBufferIntervals()
method QDBusRawType::a(uu) local.ProximitySensor.getAvailableBufferSizes()
method QDBusRawType::a(ddd) local.ProximitySensor.getAvailableDataRanges()
method QDBusRawType::a(ddd) local.ProximitySensor.getAvailableIntervals()
method QDBusRawType::(ddd) local.ProximitySensor.getCurrentDataRange()
method bool local.ProximitySensor.hwBuffering()
method QString local.ProximitySensor.id()
method uint local.ProximitySensor.interval()
method bool local.ProximitySensor.isValid()
method QDBusRawType::(tu) local.ProximitySensor.proximity()
method void local.ProximitySensor.removeDataRangeRequest(int sessionId)
method void local.ProximitySensor.requestDataRange(int sessionId, QDBusRawType::(ddd) range)
method void local.ProximitySensor.setBufferInterval(int sessionId, uint value)
method void local.ProximitySensor.setBufferSize(int sessionId, uint value)
method bool local.ProximitySensor.setDataRangeIndex(int sessionId, int rangeIndex)
method bool local.ProximitySensor.setDefaultInterval(int sessionId)
method void local.ProximitySensor.setDownsampling(int sessionId, bool value)
method void local.ProximitySensor.setInterval(int sessionId, int value)
method bool local.ProximitySensor.setStandbyOverride(int sessionId, bool value)
method bool local.ProximitySensor.standbyOverride()
method void local.ProximitySensor.start(int sessionId)
method void local.ProximitySensor.stop(int sessionId)
method QString local.ProximitySensor.type()
method QDBusVariant org.freedesktop.DBus.Properties.Get(QString interface_name, QString property_name)
method QVariantMap org.freedesktop.DBus.Properties.GetAll(QString interface_name)
method void org.freedesktop.DBus.Properties.Set(QString interface_name, QString property_name, QDBusVariant value)
method QString org.freedesktop.DBus.Introspectable.Introspect()
method QString org.freedesktop.DBus.Peer.GetMachineId()
method void org.freedesktop.DBus.Peer.Ping()


@endcode

@page developer Developer Documentation

@section driverguide Sensor driver guideline

Sensord has been built to be easily adaptable to different driver interfaces
sensor drivers may expose. However, the following generic guidelines or
requests could be made for the driver implementations:

  - Try to follow upstream conventions and standards when available.
  - Avoid busy-polling by providing interrupts from driver handle
  - Power down the sensor if nobody is attached to the driver handle.
  - Provide easy way to configure sensor (sampling range, frequenzy, etc.),
preferably using sysfs.

Kernels input (evdev) system handles most of the above quite nicely.


@section plugins Instructions For Writing Plug-ins for Sensor Framework

@subsection pluginsubsection Plug-ins

A plug-in for sensor framework can provide one (or more) of the following:

  - Device adaptor
  - Chain
  - Sensor
  - Filter

Device adaptors are responsible for interfacing the framework with data sources (generally device
drivers). Adaptors are connected to chains or sensors, which contain a varying number of filters.
Filters are used for calculating things from received samples. Sensors provide an interface to the
resulting data for client applications, while chains are used as internal datasources for
calculations shared by several sensors.


@subsubsection writingpluginsub Writing a Plug-in

A plug-in by itself is responsible for very few things. It contains an adaptor/sensor/chain/filter
(or several), and introduces them to SensorManager.

In addition, plug-ins may specify that they need another plug-in for functioning. E.g.
orientationchain (provides device orientation calculations) requires accelerometeradaptor
(provides data from accelerometer) to function.

Plugin dependencies are based on meta-names. Any plugin that requires, e.g. sampleplugin, will
specify the dependency as 'sampleplugin'. In reality, with varying platforms the real plugin could
be called sampleplugin-ascii, sampleplugin-superior, sampleplugin-inputdev, etc. See the section
'configuration files' for further details.

The naming convention is roughly:

  @a \<type_of_sensor\>\<type_of_node\>plugin-\<specifier\>

For adaptors, the specifier has been agreed to be related to the type of underlying driver
interface. Thus, we could for example have:

  accelerometeradaptorplugin-inputdev

Not all plugins currently adhere to these rules, and there is no clear rule on what to use for the
\<specifier\> in cases other than adaptors.

See the commented codes in examples/*/*plugin.[h|cpp] for plug-in construction.



@subsection writingadaptorsub Writing an Adaptor

A device adaptors is responsible for reading data from a source (usually a device driver interface)
and pushing it into the processing chain in sensor framework.

The easiest way to create a new device adaptor is to extend SysfsAdaptor. The base class will then
handle most things as long as the adaptor class itself provides the interface specific details.

What needs to be implemented:
  * Select busypoll or interrupt based approach0
  * Create and name an outputbuffer(s).
  * Function for reading and propagating data.
  * Specify metadata

@subsubsection pollparagraph Interrupt based or busypoll

Files can be monitored either in SelectMode or IntervalMode. SelectMode uses epoll() to monitor for
  interrupts, while IntervalMode just busypolls with specified delay. Using SelectMode is encouraged
  due to power saving reasons as long as driver interface provides interrupts.

In case the driver interface provides possibility to control hardware sampling frequency
  (implies SelectMode), interval() and setInterval() should be reimplemented to
  make use of the functionality.

In case the driver initiates hardware measurement when the interface is read, the IntervalMode
 handles everything related to interval handling already (except specifying the allowed values).

@subsubsection bufferparagraph Buffers

An adaptor can have any number of output buffers. Usually there is only one, but if an adaptor
monitors several filehandles, or a filehandle provides more than one type of output, it might
occasionally make sense to provide several output buffers. The buffers are named and are searched
     by listeners based on the name.

@subsubsection readingdataparagraph Reading data from driver

SysfsAdaptor provides method addPath() for registering filehandles for listening. The path can also
     be set as constructor parameter. There can be any number of paths added, but mode of the
     adaptor will be the same for all of them, as well as the interval setting.

Whenever a registered filehandle has new data (signalled either by interrupt or by timeout),
    processSample(int pathId, int fd) gets called. Data can be read from fd, and pathId is used
     to separate which file the handle points to.

The function should be implemented to read data from fd and propagate it to listeners by writing
     to buffer.

@subsubsection metadataparagraph Metadata

Metadata for adaptors should represent the capabilities of the hardware and driver.

See section 'Metadata' at the end for generic details.

See examples/sampleadaptor/ for adaptor construction.

@subsubsection adaptorshardway Implementing A Device Adaptor - the hardway


If you wish to go all the way, adaptors are expected to implement the following things:

- Provide an output buffer. Create a buffer, introduce it with

void addAdaptedSensor(const QString& name, const QString& description, RingBufferBase* buffer);

where "name" is the id for the buffer. The name is used by the next node in the filtering chain to
locate the buffer. Currently these names are hardcoded in the layers above, so if the created adaptor
is a replacement for an existing one, the same buffer name(s) should be used. Adaptors and their
buffer names and types are listed below.

- Set adaptor description with setDescription() and list possible dataRanges for the adaptor with
introduceAvailableDataRange(). DataRange options are defined as (min value, max value, accuracy).

If the adaptor supports more than one data range, introduce all ranges and implement setDataRange()
to allow the client to switch between ranges. The NodeBase class will take care of queuing the range
requests (first come, first served).

- Implement startAdaptor() and stopAdaptor(). These are leftovers from something that is not very
valid at the moment, but might come handy in the future. These are called on construction and
destruction of the adaptor (by sensormanager). One could use these for example to do some
preliminary setup for sensor connection. Current adaptors don't do much. Calling startAdaptor()
should not make the sensor consume power (unless really
necessary, think BT connection)

- Implement startSensor() and stopSensor(). These are the functions used to start the sensor
dataflow into the named buffer. They should take care of reference counting for themselves, so
that the adapted sensor (aka. the buffer) will run as long as there is someone expecting data.
The AdaptedSensorEntry class provides help in reference counting. Whenever the sensor is stopped,
it should not consume any power. (quite sensor specific what is released here and what is released
                                  with stopAdaptor()).

- Implement standby() and resume(). These are called when display blanks / unblanks. The expected
functionality is to stop on standby, and start again on resume (goal is to save power). The
difference to start and stop is that the release of hardware should be invisible to layers above.
They can adjust properties and call start/stop, but these will only take effect after the adaptor
has resumed.

- In case the rate of the sensor can be adjusted, re-implement getPollingInterval() and
setPollingInterval(). [need to revise the code here, may have too many hardcodings to original
setup to make this work in a reasonable way]



@subsection writingfilter Writing a Filter
A filter is responsible for processing data.

A new filter should be created by inheriting from QObject and Filter.

Then what need to be implemented are:
  - In the header file
        - Override the static factoryMethod() function and return a new instance of the filter
        - Define a private filter() function which can be called anything, as long as it matches
the nam-e in class constructor
   Implement the filter() function in the source file and do the necessary filtering operation
there

See examples/samplefilter/ for filter construction.



@subsection writingsensorsection Writing a Sensor


To create a new sensor, the following four steps should be done
  - implementation of AbstractSensorChannel
  - implementation of AbstractSensorChannelAdaptor
  - implementation of AbstractSensorChannelInterface
  - implementation of datatypes if sensor introduces new ones

AbstractSensorChannel is the end-node in the sensor graph.

AbstractSensorChannelAdaptor is the DBus interface of the sensor.

AbstractSensorChannelInterface is the client API for managing sensor through DBus and reading
datastream from the socket.

See examples/samplesensor/ for sensor channel construction.



@subsection writingchain Writing a Chain


A chain is responsible for adapting filters into the data flow between an adaptor and a sensor.
A new chain is created by inheriting from AbstractChain.

Then what need to be implemented are:
  - In the header file
        - Override the static factoryMethod() function and return a new instance of the chain
  - In the constructor
        - Get an instance of SensorManager
        - Get a pointer (refcounted) to the adaptor from sensor manager
        - Create a reader for the adaptor
        - Create and name output buffer
        - Create a new bin and add elements into it with names
        - Make connections between the elements in the Bin
        - Connect the reader to the adaptor
        - Setup metadata
  - Do the cleaning up in the destructor
  - Override start() function to start chain
  - Override stop() function to stop chain

See examples/samplechain/ for chain construction.


@subsection metadatasubsection Metadata


Each node (adaptor/chain/sensor, not filters) should define metadata for itself. The following
things are included:

  - Description
  - Interval range(s)
  - Data range(s)
  - Standby override sources (sort of metadata)
  - Buffer size
  - Buffer interval

@subsection metadatasubsection Description

Description should provide a human readable string that describes what is the output from the node.

@subsection rangesubsection Interval range(s)

Interval ranges define the possible rates at which this node can provide output. They are given
with min/max pairs, where any value between min and max is allowed. Resolution parameter is ignored.

Base class takes care of queuing and sorting the interval requests. Each node just needs to provide
accepted ranges and set/get methods. Interval is specified as milliseconds between samples. The
real used value will be the smallest value, ensuring that data is measured as fast as required.
This behavior can be modified for a node, in case smallest == fastest does not apply for its sources.

For adaptors, the interval should represent the real capabilities of the hardware and driver.

Chains and sensors have more possibilities. In simplest case, they just delegate the interval for
the source node by using setIntervalSource(). Then any requests are directly moved to the source
node.

If a node has more than one source whose output affect the rate of the output for the node itself,
setInterval() and interval() should be reimplemented to merge the requests into wanted rate.

@subsection datarangesubsection Data range(s)

Data ranges tell the possible min and max values and accuracy of measurements. Setting them closely
resembles interval setting. The largest difference is that datarates are set with
first-come-first-serve basis. The first requester of data range gets to keep the setting.

Currently there are no examples of sensors that would make use of multiple dataranges. For the sake
of information, the rates should be described with introduceAvailableDataRange() or taken from a
source with setIntervalSource().

@subsection standbysubsection Standby override

Default behavior is to turn off all sensors when display goes blank.

Client can request a sensor to stay on in such a case by setting the standbyOverride -property to
true. In practise, nodes specify which sources should stay on with addStandbyOverrideSource().

In adaptors, display blank will result in all filehandles being released. Setting the property to
true prevents this from happening.

The property should be used with care and only when required in order to minimize power consumption.

@subsection buffersubsection Buffer size and interval

Buffering means two things:
  - sensor driver of chip may have internal buffer for storing data. In that case adaptor will
implement setBufferSize() virtual function to configure the driver and implement
getAvailableBufferSizes() virtual function to list available buffer sizes.
  - if adaptor doesn't support buffering then sensorfw uses internal buffering to send data in
bursts to the clients. These modes are mutually exclusive for single sensor.

Buffer interval can be used to configure the time limit for long we will wait for buffer to get
full before flushing the content to the user. For driver/chip supported interval handling adaptor
will implement setBufferInterval() virtual function to configure the driver and implement
getAvailableBufferIntervals() virtual function to list available intervals.

By default buffering is disabled for client sessions.

@section configurationsection Configuration Files


Sensorfw supports multiple configuration files. There are two locations:

  (1) /etc/sensorfw/sensord.conf
  (2) /etc/sensorfw/sensord.conf.d/

Any option set in (1) will override options set in any of the files in (2). Files in (2) are given
priority based on alphanumeric order. Using double-digits as the beginning of the filename for
clarity is encouraged.

Configuration files contain sections for different HW. These sections should connect plugin
metanames (sampleadaptor) with the real plugin that should be used (sampleadaptor-inputdev).
The configuration file also contains option 'deviceId', which specifies which section should be
used. This will be removed once we have automatic detection of underlying HW in place.

The structure of the configuration files is likely to change in the future to contain better
separation between different platforms.


*/
