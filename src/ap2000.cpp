/*!
 * @file ap2000.cpp
 *
 * @section intro_sec Introduction
 *
 * This is a library for Aquaread's AP-2000 sensor.
 *
 * @section author Author
 *
 * Written by The Last Resort.
 */


#include "ap2000.hpp"


/*!
 *  @brief  Instantiates a new Ap2000Sensor class
 *  @param  sdiPin
 *          Pin number that sensor is connected
 *  @param  sensorAddress
 *          Address of the sensor
 */
Ap2000Sensor::Ap2000Sensor(uint8_t sdiPin, uint8_t sensorAddress)
{
    sensorAddr = sensorAddress;
    sensor = new SDI12(sdiPin);
}

/*!
 *  @brief  Destructor of the Ap2000Sensor class
 */
Ap2000Sensor::~Ap2000Sensor()
{
    delete sensor;
}

/*!
 *  @brief  Initiate the SDI-12 communication
 */
void Ap2000Sensor::SensorBegin()
{
    sensor->begin();
}

/*!
 *  @brief  Get the sensor general infos
 *
 *	@return Return all the general sensor infos
 */
SensorInfos Ap2000Sensor::GetSensorInfos()
{
    SensorInfos si;
    String sensorReply;

    sensor->sendCommand((String)sensorAddr + "I!");
    delay(100);
    sensorReply = sensor->readStringUntil('\n');
    sensorReply.trim();

    si.addr = sensorReply.substring(0, 1).toInt();
    si.sdiVersion = sensorReply.substring(1, 3).toFloat() / 10;
    sensorReply.substring(3, 11).toCharArray(si.vendorId, 8);
    si.vendorId[8] = '\0';
    sensorReply.substring(11, 17).toCharArray(si.model, 6);
    si.model[6] = '\0';
    si.fwVersion = sensorReply.substring(17, 20).toFloat() / 100;
    sensorReply.substring(20).toCharArray(si.serialNumber, 13);
    si.serialNumber[13] = '\0';
    sensor->clearBuffer();

    return si;
}

/*!
 *  @brief  Get the sensor general infos and display them
 *  @param  serial
 *          Serial for framework that uses HardwareSerial
 */
void Ap2000Sensor::DispSensorInfos(HardwareSerial &serial)
{
    SensorInfos si = GetSensorInfos();

    serial.println("Sensor Address: " + static_cast<String>(si.addr));
    serial.println("Sensor SDI Version: " + static_cast<String>(si.sdiVersion));
    serial.println("Vendor Id: " + static_cast<String>(si.vendorId));
    serial.println("Sensor Model: " + static_cast<String>(si.model));
    serial.println("Sensor Firmware Version: " + static_cast<String>(si.fwVersion));
    serial.println("Sensor Serial Number: " + static_cast<String>(si.serialNumber));
}

/*!
 *  @brief  Display the sensor general infos provided
 *  @param  serial
 *          Serial for framework that uses HardwareSerial
 *  @param  si
 *          SensorInfos structure to display
 */
void Ap2000Sensor::DispSensorInfos(HardwareSerial &serial, SensorInfos &si)
{
    serial.println("Sensor Address: " + static_cast<String>(si.addr));
    serial.println("Sensor SDI Version: " + static_cast<String>(si.sdiVersion));
    serial.println("Vendor Id: " + static_cast<String>(si.vendorId));
    serial.println("Sensor Model: " + static_cast<String>(si.model));
    serial.println("Sensor Firmware Version: " + static_cast<String>(si.fwVersion));
    serial.println("Sensor Serial Number: " + static_cast<String>(si.serialNumber));
}

/*!
 *  @brief  Get the sensor general infos and display them
 *  @param  serial
 *          Serial for framework that uses Serial_
 */
void Ap2000Sensor::DispSensorInfos(Serial_& serial) {
    SensorInfos si = GetSensorInfos();

    serial.println("Sensor Address: " + static_cast<String>(si.addr));
    serial.println("Sensor SDI Version: " + static_cast<String>(si.sdiVersion));
    serial.println("Vendor Id: " + static_cast<String>(si.vendorId));
    serial.println("Sensor Model: " + static_cast<String>(si.model));
    serial.println("Sensor Firmware Version: " + static_cast<String>(si.fwVersion));
    serial.println("Sensor Serial Number: " + static_cast<String>(si.serialNumber));
}

/*!
 *  @brief  Ping as SDI-12 address
 *  @param  addr
 *          Address of the sensor to ping
 *	@return Return true if the sensor replied to the ping otherwise return false
 */
bool Ap2000Sensor::IsSensorActive(uint8_t addr)
{
    String cmd = (String)addr + '!';
    for (int i = 0; i < 3; i++)
    {
        sensor->sendCommand(cmd);
        delay(100);
        if (sensor->available())
        {
            sensor->clearBuffer();
            return true;
        }
    }
    sensor->clearBuffer();
    return false;
}

/*!
 *  @brief  Request measurments to the sensor
 *  @param  mInfos
 *          char[2] that get filled with the time to wait until the measurements are ready and how many there is
 */
void Ap2000Sensor::RequestMeasurements(uint8_t *mInfos)
{
    String sensorReply;

    sensor->sendCommand((String)sensorAddr + "C!");
    delay(100);
    sensorReply = sensor->readStringUntil('\n');
    sensorReply.trim();

    mInfos[0] = sensorReply.substring(1, 4).toInt();
    mInfos[1] = sensorReply.substring(4).toInt();
    sensor->clearBuffer();
    measurementRequested = true;
}

/*!
 *  @brief  Request measurments to the sensor and wait until they are ready
 *  @param  mInfos
 *          char[2] that get filled with the time to wait until the measurements are ready and how many there is
 */
void Ap2000Sensor::RequestMeasurementsAndWaitUntilReady(uint8_t *mInfos)
{
    String sensorReply;

    sensor->sendCommand((String)sensorAddr + "C!");
    delay(100);
    sensorReply = sensor->readStringUntil('\n');
    sensorReply.trim();

    mInfos[0] = sensorReply.substring(1, 4).toInt();
    mInfos[1] = sensorReply.substring(4).toInt();
    sensor->clearBuffer();
    delay(mInfos[0] * 1000);
    measurementRequested = true;
}

/*!
 *  @brief  Request measurments to the sensor and wait until they are ready
 */
void Ap2000Sensor::RequestMeasurementsAndWaitUntilReady()
{
    String sensorReply;

    sensor->sendCommand((String)sensorAddr + "C!");
    delay(100);
    sensorReply = sensor->readStringUntil('\n');
    sensorReply.trim();
    sensor->clearBuffer();

    delay(sensorReply.substring(1, 4).toInt() * 1000);
    measurementRequested = true;
}

/*!
 *  @brief  Request measurments to the sensor and wait until they are ready
 *  @param  measurements
 *          Structure that will be provisioned with the measurements received
 *  @param  resultsExpected
 *          The number of measurements expected
 *  @param  requestAgain
 *          Force a new measurement request before getting them
 */
void Ap2000Sensor::GetSensorMeasurements(SensorMeasurements* measurements, uint8_t resultsExpected, bool requestAgain)
{
    if (!measurementRequested || !requestAgain)
        this->RequestMeasurementsAndWaitUntilReady();

    uint8_t resultsReceived = 0;
    float* mIndex = (float*)measurements;
    uint8_t cmd_number = 0;
    while (resultsReceived < resultsExpected && cmd_number <= 9)
    {
        String command = "";
        command = "";
        command += sensorAddr;
        command += "D";
        command += cmd_number;
        command += "!";
        sensor->sendCommand(command);
        uint32_t start = millis();
        while (sensor->available() < 3 && (millis() - start) < 1500)
        {
        }
        sensor->read();          // ignore the repeated SDI12 address
        char c = sensor->peek(); 
        if (c == '+' || c == '-')
        {
            sensor->read();
        }

        while (sensor->available())
        {
            char c = sensor->peek();
            if (c == '-' || (c >= '0' && c <= '9') || c == '.')
            {
                float result = sensor->parseFloat(SKIP_NONE);
                Serial.print(", ");
                Serial.print(String(result, 10));
                mIndex[resultsReceived] = result;
                if (result != -9999)
                {
                    resultsReceived++;
                }
            }
            else
            {
                sensor->read();
            }
            delay(10); // 1 character ~ 7.5ms
        }
        cmd_number++;
    }
    Serial.println();
    measurementRequested = false;
}
