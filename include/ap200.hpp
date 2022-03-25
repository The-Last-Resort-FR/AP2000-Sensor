#pragma once

#include <SDI12.h>

struct SensorInfos
{
    uint8_t addr;
    float sdiVersion;
    char vendorId[9];
    char model[7];
    float fwVersion;
    char serialNumber[14];
};

struct SensorMeasurements 
{
    float barometricPressure;
    float temperature;
    float ph;
    float oxydationReductionPotential;
    float electricalConductivity;
    float electricalConductivity20;
    float electricalConductivity25;
    float electricalResistivity;
    float salinity;
    float totalDissolvedSolid;
    float specificSeawaterGravity;
    float dissolvedOxygen;
    float dissolvedOxygenSaturation;
    float turbidity;
    float aux2;
    float nh3;
};

class Ap2000Sensor
{
private:
    SDI12* sensor;
    uint8_t sensorAddr;
    bool measurementRequested;
    bool IsSensorActive(uint8_t addr);                                  //Ping equivalent

public:
    Ap2000Sensor() = delete;
    Ap2000Sensor(const Ap2000Sensor&) = delete;
    void operator=(const Ap2000Sensor&) = delete;

    Ap2000Sensor(uint8_t sdiPin, uint8_t sensorAddress);
    ~Ap2000Sensor();

    SensorInfos GetSensorInfos();
    void DispSensorInfos(HardwareSerial& Serial);
    void DispSensorInfos(Serial_& Serial);
    void DispSensorInfos(HardwareSerial& Serial, SensorInfos& si);      //Spares execution time thus power
    void SensorBegin();
    void RequestMeasurements(uint8_t* mInfos);
    void RequestMeasurementsAndWaitUntilReady(uint8_t* mInfos);
    void RequestMeasurementsAndWaitUntilReady();
    void GetSensorMeasurements(SensorMeasurements* measurements,uint8_t resultsExpected = 16 , bool requestAgain = false);

    String units[16] = {"mBar", "°C", "ph", "mV", "uS/cm", "uS/cm", "uS/cm", "kO.cm", "PSU", "mg/L", "Sig_t", "mg/L", "%", "NTU", "none", "none"};
};
