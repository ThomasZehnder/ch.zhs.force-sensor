#include <Arduino.h>

#include "Force.h"
#include "HwInterface.h"
#include "Global.h"

clForce Force;

void clForce::setup()
{
    sensor.begin(FORCE_DOUT_PIN, FORCE_SCK_PIN);
    sensor.set_scale(calibrationFactor);
    tare();
}

void clForce::loop()
{
    if (sensor.is_ready())
    {
        Assembly.force.value = sensor.get_units(1);
    }
}

void clForce::tare()
{
    // wait_ready() (called internally by tare()/read()) blocks forever with no timeout,
    // which trips the watchdog if the HX711 is not wired up / not powered yet
    if (sensor.wait_ready_timeout(1000))
    {
        sensor.tare();
        Serial.print("Force.tare --> done, offset: ");
        Serial.println(sensor.get_offset());
    }
    else
    {
        Serial.println("Force.tare --> HX711 not responding, tare skipped");
    }
}

void clForce::calibrate(float knownForceNewton)
{
    if (knownForceNewton == 0)
    {
        return;
    }

    if (!sensor.wait_ready_timeout(1000))
    {
        Serial.println("Force.calibrate --> HX711 not responding, calibration skipped");
        return;
    }

    long rawAverage = sensor.read_average(10);
    calibrationFactor = (rawAverage - sensor.get_offset()) / knownForceNewton;
    sensor.set_scale(calibrationFactor);

    Serial.print("Force.calibrate --> new calibration factor: ");
    Serial.println(calibrationFactor);
}

void clForce::setCalibrationFactor(float factor)
{
    calibrationFactor = factor;
    sensor.set_scale(calibrationFactor);
}
