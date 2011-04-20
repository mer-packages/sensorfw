/**
   @file nodebase.h
   @brief Base class for all nodes in filtering chain

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

   This file is part of Sensord.

   Sensord is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   Sensord is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Sensord.  If not, see <http://www.gnu.org/licenses/>.
   </p>
 */

#ifndef NODEBASE_H
#define NODEBASE_H

#include <QObject>
#include <QString>
#include <QList>
#include "datarange.h"
#include "logging.h"

class RingBufferReaderBase;
class RingBufferBase;

/**
 * Base class for all nodes in sensord framework filtering chain.
 * Provides common functions for handling node metadata.
 */
class NodeBase : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(NodeBase)
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(bool standbyOverride READ standbyOverride)
    Q_PROPERTY(unsigned int interval READ getInterval)
    Q_PROPERTY(QString id READ id)
    Q_PROPERTY(bool isValid READ isValid)

protected:
    /**
     * Constructor.
     *
     * @param id node ID.
     * @param parent parent object.
     */
    NodeBase(const QString& id, QObject* parent = 0);

    /**
     * Destructor.
     */
    virtual ~NodeBase();

public:
    /**
     * Idenfication string for the sensor node
     *
     * @return node ID.
     */
    const QString& id() const;

    /**
     * Is object succesfully initialized.
     *
     * @return is object successfully initialized.
     */
    bool isValid() const;

public Q_SLOTS:
    /**
     * Get the description for this node.
     *
     * @return Descriptive string.
     */
    const QString& description() const;

    /**
     * Remove a range request.
     *
     * @param sessionId ID of the session whose request to remove.
     */
    void removeDataRangeRequest(int sessionId);

    /**
     * Get list of available data ranges for this node.
     *
     * @return List of available data ranges for this node.
     */
    const QList<DataRange>& getAvailableDataRanges() const;

    /**
     * Get the currently active DataRange for this node.
     *
     * @return DataRangeRequest currently in use. If no requests are set
     *         id will be set to -1.
     */
    DataRangeRequest getCurrentDataRange() const;

    /**
     * Places a request for the given data range into queue. The range
     * will be activated once all earlier requests have been released.
     *
     * If the range is not valid, the request will be dropped
     * immediately.
     *
     * @param sessionId session ID for the client making the request.
     * @param range The requested data range.
     */
    void requestDataRange(int sessionId, const DataRange& range);

    /**
     * Places a request for the given data range by index into queue.
     * The range will be activated once all earlier requests have been
     * released.
     *
     * If the range is not valid, the request will be dropped immediately.
     *
     * @param sessionId session ID for the client making the request.
     * @param rangeIndex The requested data range index
     * @return false is returned if given index is out-of-bounds.
     */
     bool setDataRangeIndex(int sessionId, int rangeIndex);

    /**
     * Tells whether the node is in standbyOverride mode. If \c true, the
     * dataflow from the sensor will continue even if the device display
     * goes blank. Any client making the request for a sensor will make
     * all clients of the sensor experience the same behavior.
     *
     * Note: Due to the fact that any other client may request this
     * property for another sensor, whose processing chain is partly
     * shared with the sensor this client is using, it is possible to
     * receive samples only from some sources (i.e. this sensor
     * has the property off, the one one has it on --> we get samples
     * from the shared parts). This may lead to some unexpected behavior.
     *
     * Return value is based on whether all source nodes are or local
     * implementation is true.
     *
     * @return \c true if standyOverride is on, \c false otherwise.
     */
    bool standbyOverride() const;

    /**
     * Sets a request for standbyOverride property.
     *
     * @param sessionId ID of the session making the request.
     * @param override Whether to request (\c true) or reset (\c false).
     * @return \c true if local implementation or all sources return true.
     *         \c false otherwise.
     */
    bool setStandbyOverrideRequest(int sessionId, bool override);

    /**
     * Returns list of possible intervals for the sensor. If \c min and
     * \c max value are the same, the value is discrete. If they are
     * different, any value between \c min and \c max can be requested.
     * \c Resolution can be ignored.
     *
     * @return List of possible intervals in milliseconds for this node.
     */
    const QList<DataRange>& getAvailableIntervals() const;

    /**
     * Set interval request for the node. Acceptable values are listed by
     * #getAvailableIntervals().
     *
     * @param sessionId Session ID.
     * @param value interval value is milliseconds.
     * @return was request succesful.
     */
    bool setIntervalRequest(int sessionId, unsigned int value);

    /**
     * Request default interval for the node.
     *
     * @param sessionId Session ID.
     * @return was request succesful.
     */
    bool requestDefaultInterval(int sessionId);

    /**
     * Returns the default interval value for this node.
     *
     * @return The default interval.
     */
    unsigned int defaultInterval() const;

    /**
     * Remove interval requests for given session.
     *
     * @param sessionId Session ID.
     */
    void removeIntervalRequest(int sessionId);

    /**
     * Return the interval.
     *
     * @return current interval.
     */
    unsigned int getInterval() const;

    /**
     * Return the interval of given session.
     *
     * @param sessionId Session ID.
     * @return interval requested by given session.
     */
    unsigned int getInterval(int sessionId) const;

    /**
     * Returns list of available buffer sizes. The list is ordered by
     * efficiency of the size.
     *
     * @param hwSupported Is the buffering supported by driver.
     * @return The list of supported buffer sizes ordered by efficiency.
     */
    virtual IntegerRangeList getAvailableBufferSizes(bool& hwSupported) const;

    /**
     * Returns list of available buffer intervals.
     *
     * @param hwSupported Is the buffering supported by driver.
     * @return The list of supported buffer intervals.
     */
    virtual IntegerRangeList getAvailableBufferIntervals(bool& hwSupported) const;

    /**
     * Get current buffersize of the node.
     *
     * @return current buffersize.
     */
    virtual unsigned int bufferSize() const { return 0; }

    /**
     * Get current buffer interval of the node.
     *
     * @return current buffer interval in milliseconds.
     */
    virtual unsigned int bufferInterval() const { return 0; }

    /**
     * Set buffersize for given session.
     *
     * @param sessionId Session ID.
     * @param value buffer size.
     * @return was size set succesfully.
     */
    bool setBufferSize(int sessionId, unsigned int value);

    /**
     * Clear buffer size requests of given session.
     *
     * @param sessionId Session ID.
     * @return was buffer size request cleared.
     */
    bool clearBufferSize(int sessionId);

    /**
     * Set buffer interval for given session.
     *
     * @param sessionId Session ID.
     * @param value interval in milliseconds.
     * @return was interval se succesfully.
     */
    bool setBufferInterval(int sessionId, unsigned int value);

    /**
     * Clear buffer interval requests of given session.
     *
     * @param sessionId Session ID.
     * @return was buffer interval request cleared.
     */
    bool clearBufferInterval(int sessionId);

    /**
     * Removes session related associates from the node.
     *
     * @param sessionId Session ID.
     */
    virtual void removeSession(int sessionId);

Q_SIGNALS:
    /**
     * Property value has changed signal.
     *
     * @param name property name.
     */
    void propertyChanged(const QString& name);

protected:
    /**
     * Set object validity state.
     *
     * @param valid is object valid.
     */
    void setValid(bool valid);

    /**
     * Sets up a source for this node.
     *
     * @param source source node.
     * @param bufferName source buffer name to connect to.
     * @param reader reader to connect to source buffer.
     * @return was connection succesful.
     */
    bool connectToSource(NodeBase* source, const QString& bufferName, RingBufferReaderBase* reader);

    /**
     * Remove source for this node.
     *
     * @param source source node.
     * @param bufferName source buffer name to disconnect from.
     * @param reader reader to disconnect from source buffer.
     * @return was disconnection succesful.
     */
    bool disconnectFromSource(NodeBase* source, const QString& bufferName, RingBufferReaderBase* reader);

    /**
     * Validates the metadata setup for the node. To pass, exactly one
     * of the following conditions must be fullfilled for each propagative
     * property.
     * <ol>
     *   <li>Node on the previous layer has been marked as a source</li>
     *   <li>At least one value has been given for this node</li>
     * </ol>
     *
     * This function should be placed to the end of each node constructor.
     *
     * @return \c true on valid metadata setup, \c false otherwise.
     */
    bool isMetadataValid() const;

    /**
     * Set the description for the node.
     *
     * @param str The description string to set.
     */
    void setDescription(const QString& str);

    /**
     * Introduce a new available range. Locally defined range will
     * override any ranges given by previous layers in the filtering
     * chain.
     *
     * This function is expected to be used only during node construction.
     * Introduced ranges can not be removed.
     *
     * @param range The range to add.
     */
    void introduceAvailableDataRange(const DataRange& range);

    void introduceAvailableDataRanges(const QString& typeName);

    /**
     * Sets the node to request range information from in case local
     * values have not been set.
     *
     * @param node The node to request range metadata from.
     */
    void setRangeSource(NodeBase* node);

    /**
     * Sets the current datarange. Overwrite for nodes which have more
     * than one possible range and are responsible for actions to change
     * it. For nodes which merely get several options from previous nodes
     * this won't get invoked.
     *
     * @param range The range to set
     * @param sessionId The session Id
     * @return \c true on succesfull set, \c false otherwise.
     */
    virtual bool setDataRange(const DataRange& range, int sessionId);

    /**
     * Sets the standbyOverride value for the node. This is the base
     * implementation, which should be reimplemented for any node that
     * wishes to provide real functionality.
     *
     * @param override \c true to set standbyOverride on, \c false to
     *        remove the request.
     * @return Whether request was successfull.
     */
    virtual bool setStandbyOverride(bool override);

    /**
     * Adds a new node to the list of nodes that standbyOverride calls
     * are forwarded to. Each node in the list gets the requests sent
     * to this node.
     *
     * Should be called by nodes that need information on standbyOverride
     * from nodes in the previous layer (i.e. everything but adaptors).
     *
     * @param node Node to append to the list.
     */
    void addStandbyOverrideSource(NodeBase* node);

    /**
     * Add a new interval to list of locally provided ones
     *
     * @param interval new interval range for the node.
     */
    void introduceAvailableInterval(const DataRange& interval);

    void introduceAvailableIntervals(const QString& typeName);

    /**
     * Return the locally valid interval.
     *
     * @return locally valid interval.
     */
    virtual unsigned int interval() const;

    /**
     * Set given interval for the node.
     *
     * @param value interval in milliseconds.
     * @param sessionId Session ID.
     * @return was interval set succesfully.
     */
    virtual bool setInterval(unsigned int value, int sessionId);

    /**
     * Does node have locally set interval.
     *
     * @return does node have locally set interval.
     */
    bool hasLocalInterval() const;

    /**
     * Evalutes the requests for interval, and returns the one that should
     * be used. In case there are no requests, the default interval (set by
     * setDefaultInterval()) is returned.
     *
     * This implementation considers smallest non-negative interval request
     * as the winner. Reimplement for nodes that need to use different
     * approach.
     *
     * <b>Note that this approach has been considered 'proper' by design.
     * Consider carefully what the consequences may be for other parts of
     * the system if you reimplement this.</b>
     *
     * @param sessionId Reference to parameter for storing the winning
     *                  sessionId. Will be set to \c -1 if no active
     *                  requests are in place.
     * @return The winning interval.
     */
    virtual unsigned int evaluateIntervalRequests(int& sessionId) const;

    /**
     * Node to fetch interval from
     *
     * @param node interval source.
     */
    void setIntervalSource(NodeBase* node);

    /**
     * Sets the default interval value. The set value must always be a
     * valid setting.
     *
     * @param value Value to use as default interval.
     * @return was interval set succesfully.
     */
    bool setDefaultInterval(unsigned int value);

    /**
     * Validate an interval request.
     *
     * @param value Value to validate.
     * @return is valid request.
     */
    bool isValidIntervalRequest(unsigned int value) const;

    /**
     * Find buffer with given name.
     *
     * @param name buffer name to locate.
     * @return pointer to buffer or NULL if not found.
     */
    virtual RingBufferBase* findBuffer(const QString& name) const = 0;

    /**
     * Set buffer size. Nodes subclasses supporting buffering needs to
     * reimplement this.
     *
     * @param value buffer size.
     * @return was buffer size set succesfully.
     */
    virtual bool setBufferSize(unsigned int value);

    /**
     * Set buffer interval. Nodes subclasses supporting buffering needs to
     * reimplement this.
     *
     * @param value buffer interval.
     * @return was buffer interval set succesfully.
     */
    virtual bool setBufferInterval(unsigned int value);

    QMap<int, unsigned int> m_intervalMap;    /**< active interval requests for sessions */

    unsigned int            m_bufferSize;     /** buffer size */
    unsigned int            m_bufferInterval; /** buffer interval */

private:
    /**
     * Returns whether the class defines its own output data range, or
     * whether it uses the values from previous layer.
     *
     * @return \c true if range defined locally, \c false otherwise
     */
    bool hasLocalRange() const;

    /**
     * Re-evaluate buffer size for the node.
     *
     * @return was buffer size changed.
     */
    bool updateBufferSize();

    /**
     * Parse data range list from given text input.
     *
     * @param input text input.
     * @param defaultResolution default resolution to set to created DataRange unless explicitly set.
     * @return parsed data range list.
     */
    DataRangeList parseDataRangeList(const QString& input, int defaultResolution) const;

    /**
     * Re-evaluate buffer interval for the node.
     *
     * @return was buffer interval changed.
     */
    bool updateBufferInterval();

    QString                 m_description; /**< node description */

    QList<DataRange>        m_dataRangeList; /**< available data ranges */
    QList<DataRangeRequest> m_dataRangeQueue; /**< data range requests */
    NodeBase*               m_dataRangeSource; /**< data range source node */

    QList<NodeBase*>        m_standbySourceList; /** standbyoverride source nodes */
    QList<int>              m_standbyRequestList; /** standbyoverride requests */
    QList<DataRange>        m_intervalList;   /**< available intervals */
    NodeBase*               m_intervalSource; /**< interval sources */
    bool                    m_hasDefault;     /**< does node have locally set interval */
    unsigned int            m_defaultInterval; /**< locally set interval */

    QList<NodeBase*>        m_sourceList; /**< source nodes */

    //Oldest session wins for these:
    QMap<int, unsigned int> m_bufferSizeMap; /**< buffersize requests for sessions. */
    QMap<int, unsigned int> m_bufferIntervalMap; /**< buffer interval requests for sessions. */

    const DataRangeRequest  DEFAULT_DATA_RANGE_REQUEST; /**< default data range request */

    QString                 id_; /**< node ID */
    bool                    isValid_; /**< is node correctly initialized */
};

#endif
