#ifndef FORCE_H
#define FORCE_H

#include <HX711.h>

#define EARTH_GRAVITY_MPS2 9.80665f

class clForce
{
public:
    void setup();
    void loop();

    void tare();
    void calibrate(float knownForceNewton);
    void setCalibrationFactor(float factor);

private:
    HX711 sensor;
    float calibrationFactor = 1.0; // counts per Newton, needs calibration against a known reference force
};

extern clForce Force;

#endif // FORCE_H
