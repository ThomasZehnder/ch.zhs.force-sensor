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
    sensor.tare();
}

void clForce::setCalibrationFactor(float factor)
{
    calibrationFactor = factor;
    sensor.set_scale(calibrationFactor);
}
