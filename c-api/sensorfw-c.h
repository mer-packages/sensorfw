/**
   @file sensorfw-c.h
   @brief C-API for sensor framework. <b>NOTE THAT THIS HAS NOT BEEN IMPLEMENTED YET<b>

    @todo
    <ul>
    <li>Querying and setting values for Data range</li>
    <li>Querying possible values for Interval and Data range</li>
    <li>Control / Listen separation?</li>
    <li>Generic type for callbacks, or callback must know what to expect?</li>
    </ul>

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

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

#ifndef SENSORFW_CAPI
#define SENSORFW_CAPI

/**
 * @brief Structure containing interval information for sensor.
 *
 * if \c min and \c max are set to same value, the value is explicit. If they
 * are different, any value in the range \c [min,max] can be requested.
 */
typedef struct {
    int min; ///< Minimum value
    int max; ///< Maximum value
} sensorfw_interval_t;

/**
 * @brier Structure containing data range information for sensor.
 */
typedef struct {
    int min; ///< Minimum value
    int max; ///< Maximum value
    int accuracy; ///< Minimal detected change
} sensorfw_range_t;

/**
 * @brief Initialises the sensor for operation.
 *
 * This function must be run before attempting to request a session for a sensor.
 * Plugin loading, type registration etc. will be done by this function.
 * @param sensor_name Name of the sensor to initialise.
 * @return \c true on success, \c false on failure
 * @todo Provide information on sensor names
 */
bool sensorfw_init(const char* sensor_name);

/**
 * @brief Opens a session for a sensor.
 *
 * This call provides the client with a session ID, through which the client can
 * interact with the sensor.
 * @param sensor_name Name of the sensor we wish to open.
 * @return session ID for the sensor. \c -1 on failure.
 */
int sensorfw_open_session(const char* sensor_name);

/**
 * @brief Closes a sensor session.
 *
 * Sensor will be released. It must be reopened with sensorfw_open_session for
 * any further interaction.
 * @param sessionId Session ID to run this request on.
 * @return \c true if successfull, \c false if failed or invalid session ID.
 */
bool sensorfw_close_session(int sessionId);

/**
 * @brief Starts sensor dataflow.
 * @param sessionId Session ID to run this request on.
 * @return \c true if successfull, \c false if failed or invalid session ID.
 */
bool sensorfw_start_sensor(int sessionId);

/**
 * @brief Stops sensor dataflow.
 * @param sessionId Session ID to run this request on.
 * @return \c true if successfull, \c false if failed or invalid session ID.
 */
bool sensorfw_stop_sensor(int sessionId);

/**
 * @brief Tells whether sensor is running or not.
 * @param sessionId Session ID to run this request on.
 * @return \c true if running, \c false if not running or invalid session ID.
 */
bool sensorfw_running(int sessionId);

/**
 * @brief Tells the sample interval for the sensor.
 *
 * Sample interval is defined as the minimum amount of milliseconds that will
 * pass between consequent samples. Depending on sensor, this might be nearly
 * constant, or might depend on user interaction (i.e. no samples if environment
 * is not changing)
 * @param sessionId Session ID to run this request on.
 * @return Milliseconds between samples.
 */
int sensorfw_get_interval(int sessionId);

/**
 * @brief Sets the sample interval for sensor.
 *
 * See sensorfw_get_interval.
 * 
 * @param sessionId Session ID to run this request on.
 * @param interval Milliseconds to wait between measurements.
 * @return \c true on success, \c false on failure or invalid session ID.
 */
bool sensorfw_set_interval(int sessionId, int interval);

/**
 * @brief Tells whether sensor has the standby override property set.
 *
 * Standby override property defines whether the sensor should keep running even
 * if display turns off. Not all sensors support this (due to architectural
 * limitations), and due to power management reasons the property should be kept
 * at false whenever possible. Every sensor will default to \c false for the
 * property, but any client making a request will affect all other clients as
 * well.
 * @param sessionId Session ID to run this request on.
 * @return \c true on success, \c false on failure or invalid session ID.
 */
bool sensorfw_get_standby_override(int sessionId);

/**
 * @brief Set the standby override property.
 *
 * See sensorfw_get_standby_override.
 * @param sessionId Session ID to run this request on.
 * @param override \c true to request the property, \c false to reset the request.
 * @return \c true on success, \c false on failure or invalid session ID.
 */
bool sensorfw_set_standby_override(int sessionId, bool override);

/**
 * @brief Tells a textual description of the sensor.
 *
 * @param sessionId Session ID to run this request on.
 * @param description Will be set to point to the request. Valid until the next
 *        request.
 * @return \c true on success, \c false on failure or invalid session ID.
 */
bool sensorfw_get_description(int sessionId, char** description);

/**
 * @brief Registers a callback function to handle sensor output.
 *
 * @todo Description of callback requirements.
 * 
 * @param sessionId Session ID to run this request on.
 * @param cb_func Pointer to function to use as callback.
 * @return \c true on success, \c false on failure or invalid session ID.
 */
bool sensorfw_register_callback(int sessionId, void (*cb_func)(void *data));

/**
 * @brief Prepares the sensor for calibration.
 *
 * Often this means resetting the current calibration state. Implementation is
 * sensor specific.
 *
 * @param sessionId Session ID to run this request on.
 * @return \c true if successfull, \c false if failed or invalid for this sensor.
 * @todo Provide reference to information on which sensors this call makes sense.
 */
bool sensorfw_prepare_for_calibration(int sessionId);

/**
 * @brief Returns the last error that has occurred for sensor.
 * @param sessionId Session ID to run this request on.
 * @param error_string If given, will be set to verbal description of the error.
 *        Can be referenced until the next error occurs.
 * @return Numerical code for the error that occurred
 * @todo Provide enums for error numbers.
 */
int sensorfw_last_error(int sessionId, char** error_string);

#endif // SENSORFW_CAPI
