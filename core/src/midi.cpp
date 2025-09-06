/* Midi functionality - built on top of RtMidi */

#include <iostream>
#include <cstdlib>
#include <string>
#include <cmath>
#include <list>
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

static std::atomic<bool> gainChanged = false;  // Used to signal main_window.cpp

// Tune step is in Hz
static std::atomic<int> stepIndex = 7;
static std::list<double> steps = {1.0, 10.0, 25.0, 50.0, 100.0, 250.0, 500.0, 1.0e3, 5.0e3, 6.25e3, 9.0e3, 1.0e4, 1.25e4, 1.0e5, 2.5e5, 5.0e5, 1.0e6};
static std::atomic<double> tuneStep = 1000.0;
static std::atomic<double> tuneFreqChangedBy = 0.0;


// RtMidi Callback
void midi_msg_cb(double deltatime, std::vector<unsigned char>* message, void* /*userData*/) {
    unsigned int nBytes = message->size();
    // for (unsigned int i = 0; i < nBytes; i++)
    //     std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
    // if (nBytes > 0)
    //     std::cout << "stamp = " << deltatime << std::endl;
    std::string val;

    switch((int)message->at(0))
    {
        case 144: // Notes
        {
            switch((int)message->at(1))
            {
                // Buttons
                case 1:     // Step+
                case 20:
                {
                    // Only respond to "Note On" or Button Pressed
                    if ((int)message->at(2) == 127)
                    {
                        int idx = stepIndex.load() + 1;

                        if (idx < 0) {
                            idx = 0;
                        }

                        if (idx > (steps.size() - 1)) {
                            idx = steps.size() - 1;
                        }

                        tuneStep.store(*std::next(steps.begin(), idx));
                        stepIndex.store(idx);

                        std::string msg = "+Step Size = " + std::to_string(tuneStep.load());
                        flog::info(msg.c_str());
                    }
                    break;
                }

                case 2:     // Step-
                case 21:
                {
                    // Only respond to "Note On" or Button Pressed
                    if ((int)message->at(2) == 127)
                    {
                        int idx = stepIndex.load() - 1;

                        if (idx < 0) {
                            idx = 0;
                        }

                        if (idx > (steps.size() - 1)) {
                            idx = steps.size() - 1;
                        }

                        tuneStep.store(*std::next(steps.begin(), idx));
                        stepIndex.store(idx);

                        std::string msg = "+Step Size = " + std::to_string(tuneStep.load());
                        flog::info(msg.c_str());
                    }
                    break;
                }

                default:
                    break;
            }
            break;
        }

        case 176: // Knobs and wheels
        {

            switch((int)message->at(1))
            {
                // Case Tune Wheel
                case 1:
                {
                    switch((int)message->at(2))
                    {
                        case 43: // ---
                            tuneFreqChangedBy.store(tuneStep.load() * -5.0);
                            break;
                        case 53: // --
                            tuneFreqChangedBy.store(tuneStep.load() * -2.0);
                            break;
                        case 63: // -
                            tuneFreqChangedBy.store(tuneStep.load() * -1.0);
                            break;
                        case 65: // +
                            tuneFreqChangedBy.store(tuneStep.load() * 1.0);
                            break;
                        case 75: // ++
                            tuneFreqChangedBy.store(tuneStep.load() * 2.0);
                            break;
                        case 85: // +++
                            tuneFreqChangedBy.store(tuneStep.load() * 5.0);
                            break;
                        default:
                            break;
                    }
                    break;
                }



                // Knobs
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
                    gainChanged.store(true);
                    break;

                case 13: // IF Gain
                    currentIfGainKnob.store((int)message->at(2));
                    gainChanged.store(true);
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
            break;
        }

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
    float sf = (float)((maxValue - minValue)) / 127.0f;
    return(std::round(((float)knobValue * sf) + (float)minValue));
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

/* Returns the current tuning step size */
double Midi::getTuneStep() {
    return tuneStep.load();
}

/* Wheel: If true, value contains the new frequency */
bool Midi::getTuneWheel(double *frequency) {
    bool changed = false;

    if(Midi::midiDisabled) return false;

    double freqDelta = tuneFreqChangedBy.load();
    if(freqDelta > 0.5 || freqDelta < 0.5)
    {
        *frequency = *frequency + freqDelta;
        tuneFreqChangedBy.store(0.0);
        changed = true;
    }

    std::string msg = "Freq Changed By = " + std::to_string(freqDelta);
    flog::info(msg.c_str());

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

/* Check to see if any of the gains changed */
bool Midi::checkGainChanged() {

    if(Midi::midiDisabled) return false;

    if(gainChanged.load())
    {
        gainChanged.store(false);
        return true;
    }

    return false;
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

