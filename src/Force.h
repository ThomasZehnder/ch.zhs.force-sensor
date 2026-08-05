#ifndef FORCE_H
#define FORCE_H

#include <HX711.h>

class clForce
{
public:
    void setup();
    void loop();

    void tare();
    void setCalibrationFactor(float factor);

private:
    HX711 sensor;
    float calibrationFactor = 1.0; // counts per Newton, needs calibration against a known reference force
};

extern clForce Force;

#endif // FORCE_H
