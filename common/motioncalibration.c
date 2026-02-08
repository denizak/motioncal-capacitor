#include "motioncalibration.h"

MotionCalibration_t motioncal;

void updateBValue(float B) {
    motioncal.B = B * 2;
}