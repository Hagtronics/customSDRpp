/* Midi functionality - built on top of RtMidi */

#include <iostream>
#include <cstdlib>
#include <string>
//#include <atomic> included in midi.h
#include "midi.h"
#include <utils/flog.h>


// Class functions

// Wheel is = 0, 1, 2 or 5
static std::atomic<int> currentTuneWheel = 1;

// Knob positions = 0 to 127
static std::atomic<int> currentZoomKnob = 0;
static std::atomic<int> currentVolumeKnob = 0;
static std::atomic<int> currentSquelchKnob = 0;
static std::atomic<int> currentRfGainKnob = 0;
static std::atomic<int> currentIfGainKnob = 0;
static std::atomic<int> currentPanHKnob = 0;
static std::atomic<int> currentPanLKnob = 0;

// Tune step is in Hz
static std::atomic<int> currentTuneStep = 1000;

void midi_msg_cb(double deltatime, std::vector<unsigned char>* message, void* /*userData*/) {
    unsigned int nBytes = message->size();
    // for (unsigned int i = 0; i < nBytes; i++)
    //     std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
    // if (nBytes > 0)
    //     std::cout << "stamp = " << deltatime << std::endl;

    //flog::info("MIDI CB Called -> MSG -> VAL");
    //std::string msg = std::to_string((int)message->at(1));
    //std::string val = std::to_string((int)message->at(2));
    ///flog::info(msg.c_str());
    //flog::info(val.c_str());
    std::string val;

    switch((int)message->at(1))
    {
        case 9: // Zoom
            currentZoomKnob.store((int)message->at(2));
            break;
        case 10: // Volume
            currentVolumeKnob.store((int)message->at(2));
            break;
        case 11: // Squelch
            currentSquelchKnob.store((int)message->at(2));
            break;
        case 12: // RF Gain
            currentRfGainKnob.store((int)message->at(2));
            break;
        case 13: // IF Gain
            currentIfGainKnob.store((int)message->at(2));
            break;
        case 14: // Pan H
            currentPanHKnob.store((int)message->at(2));
            break;
        case 15: // Pan L
            currentPanLKnob.store((int)message->at(2));
            break;

        default:
            break;

    }

}


// Takes a raw knob value (0-127) and scales it between the min and max given
// returns the scaled value.
float Midi::scaleKnob(int knobValue, float minValue, float maxValue){
    float sf = (maxValue - minValue) / 127.0f;
    return( ((float)knobValue * sf) + minValue );
}

int Midi::scaleKnob(int knobValue, int minValue, int maxValue){
    float sf = (maxValue - minValue) / 127;
    return(((int)((float)knobValue * sf)) + minValue );
}


// Inits the MIDI controller by name (or partial part of name), Returns true if all went OK.
bool Midi::init(std::string desired_controller_name) {
    try {
        midiin = new RtMidiIn();

        flog::info("Looking for Midi Port,");
        flog::info(desired_controller_name.c_str());

        unsigned int nPorts = midiin->getPortCount();

        for (unsigned int i = 0; i < nPorts; i++) {
            std::string portName = midiin->getPortName(i);
            flog::info("Midi Port Name Found,");
            flog::info(portName.c_str());

            if (portName.find(desired_controller_name) != std::string::npos) {
                // Found the name - try to open the port number
                midiin->openPort(i);

                // Set our callback function.  This should be done immediately after
                // opening the port to avoid having incoming messages written to the
                // queue instead of sent to the callback function.
                midiin->setCallback(&midi_msg_cb);

                // TODO - Figure out what this really does....
                // Don't ignore sysex, timing, or active sensing messages.
                midiin->ignoreTypes(false, false, false);

                // All set, so return
                Midi::midiDisabled = false;

                return true;
            }
        }
    }
    catch (RtMidiError& error) {
        error.printMessage();
    }

    // If here then port was NOT opened properly, clean up
    delete midiin;
    return false;
}


/* Wheel: If true, value contains the number of tune steps to take. */
bool Midi::getTune(float *value) {
    bool changed = false;

    if(Midi::midiDisabled) return false;

    // Finally clear flags and values to reset the control.
    return changed;
}

/* Knob: If true, value is the knob position 0-127 */
bool Midi::getZoom(float *scaledValue, float minValue, float maxValue) {
    static int lastZoomKnob = 0;
    bool changed = false;

    if(Midi::midiDisabled) return false;

    int current = currentZoomKnob.load();

    if(lastZoomKnob != current){
        *scaledValue = Midi::scaleKnob(current, minValue, maxValue);
        lastZoomKnob = current;
        changed = true;
    }

    return changed;
}

/* Knob: If true, value is the knob position 0-127 */
bool Midi::getVolume(float *scaledValue, float minValue, float maxValue) {
    static int lastVolumeKnob = 0;
    bool changed = false;

    if(Midi::midiDisabled) return false;

    int current = currentVolumeKnob.load();

    if(lastVolumeKnob != current){
        *scaledValue = Midi::scaleKnob(current, minValue, maxValue);
        lastVolumeKnob = current;
        changed = true;
    }

    return changed;
}

/* Knob: If true, value is the knob position 0-127 */
bool Midi::getSquelch(float *scaledValue, float minValue, float maxValue) {
    static int lastSquelchKnob = 0;
    bool changed = false;

    if(Midi::midiDisabled) return false;

    int current = currentSquelchKnob.load();

    if(lastSquelchKnob != current){
        *scaledValue = Midi::scaleKnob(current, minValue, maxValue);
        lastSquelchKnob = current;
        changed = true;
    }

    return changed;
}

/* Knob: If true, value is the knob position 0-127 */
bool Midi::getRfGain(int *scaledValue, int minValue, int maxValue) {
    static int lastRfGainKnob = 0;
    bool changed = false;

    if(Midi::midiDisabled) return false;

    int current = currentRfGainKnob.load();

    if(lastRfGainKnob != current){
        *scaledValue = Midi::scaleKnob(current, minValue, maxValue);
        lastRfGainKnob = current;
        changed = true;
    }

    return changed;
}

/* Knob: If true, value is the knob position 0-127 */
bool Midi::getIfGain(int *scaledValue, int minValue, int maxValue) {
    static int lastIfGainKnob = 0;
    bool changed = false;

    if(Midi::midiDisabled) return false;

    int current = currentIfGainKnob.load();

    if(lastIfGainKnob != current){
        *scaledValue = Midi::scaleKnob(current, minValue, maxValue);
        lastIfGainKnob = current;
        changed = true;
    }

    return changed;
}

/* Knob: If true, value is the knob position 0-127 */
bool Midi::getPanH(float *scaledValue, float minValue, float maxValue) {
    static int lastPanHKnob = 0;
    bool changed = false;

    if(Midi::midiDisabled) return false;

    int current = currentPanHKnob.load();

    if(lastPanHKnob != current){
        *scaledValue = Midi::scaleKnob(current, minValue, maxValue);
        lastPanHKnob = current;
        changed = true;
    }

    return changed;
}

/* Knob: If true, value is the knob position 0-127 */
bool Midi::getPanL(float *scaledValue, float minValue, float maxValue) {
    static int lastPanLKnob = 0;
    bool changed = false;

    if(Midi::midiDisabled) return false;

    int current = currentPanLKnob.load();

    if(lastPanLKnob != current){
        *scaledValue = Midi::scaleKnob(current, minValue, maxValue);
        lastPanLKnob = current;
        changed = true;
    }

    return changed;
}

/* Button: If true, button was pressed */
bool Midi::getStepPlus() {
    if(Midi::midiDisabled) return false;
    return false;
}

/* Button: If true, button was pressed */
bool Midi::getStepMinus() {
    if(Midi::midiDisabled) return false;
    return false;
}

