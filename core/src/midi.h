#pragma once

#include <atomic>
#include "RtMidi.h"

/* MIDI Functionality Header */

class Midi {
public:
    bool init(std::string desired_controller_name);
    bool getTune(float &value);
    bool getZoom(float *value, float scale_min, float scale_max);
    bool getVolume(float *value, float scale_min, float scale_max);
    bool getSquelch(float &value, float minValue, float maxValue);
    bool getRfGain(float &value, float minValue, float maxValue);
    bool getIfGain(float &value, float minValue, float maxValue);
    bool getPanH(float &value, float minValue, float maxValue);
    bool getPanL(float &value, float minValue, float maxValue);
    bool getStepPlus();
    bool getStepMinus();

private:
    RtMidiIn *midiin = 0;
    bool midiDisabled = true;

    float scaleKnob(int knob, float min, float max);
};
