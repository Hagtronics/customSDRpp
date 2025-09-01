#pragma once
// #include "rtmidi\RtMidi.h"
#include "RtMidi.h"

/* MIDI Functionality Header */

class Midi {
public:
    bool init(std::string desired_controller_name);
    bool getTune(float &value);
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
    bool message_ready = false;

    // Wheel is = 0, 1, 2 or 5
    float currentTuneWheel = 0;

    // Knob positions = 0 to 127
    int currentVolumeKnob = 0;
    int currentSquelchKnob = 0;
    int currentRfGainKnob = 0;
    int currentIfGainKnob = 0;
    int currentPanHKnob = 0;
    int currentPanLKnob = 0;

    // Tune step is in Hz
    int currentTuneStep = 1000;
    float scaleKnob(int knob, float min, float max);
    //void midi_msg_cb(double deltatime, std::vector<unsigned char>* message, void* /*userData*/);
};
