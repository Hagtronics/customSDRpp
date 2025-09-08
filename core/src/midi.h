#pragma once

#include <atomic>
#include "RtMidi.h"

/* MIDI Functionality Header */

class Midi {
public:
    bool init(std::string desired_controller_name);
    bool getTuneWheel(double *value);
    bool getZoom(float *scaledValue, float scale_min, float scale_max);
    bool getVolume(float *scaledValue, float scale_min, float scale_max);
    bool getSquelch(float *scaledValue, float minValue, float maxValue);
    bool getPanH(float *scaledValue, float minValue, float maxValue);
    bool getPanL(float *scaledValue, float minValue, float maxValue);
    bool getRfGain(int *scaledValue, int minValue, int maxValue);
    bool getIfGain(int *scaledValue, int minValue, int maxValue);
    bool getBandwidth(float *scaledValue, float minValue, float maxValue);
    bool checkGainChanged();
    bool checkBandwidthChanged();
    bool checkSquelchChanged();
    bool getStepPlus();
    bool getStepMinus();
    double getTuneStep();

private:
    RtMidiIn *midiin = 0;
    bool midiDisabled = true;

    float scaleKnob(int knob, float min, float max);
    int scaleKnob(int knob, int min, int max);
};
