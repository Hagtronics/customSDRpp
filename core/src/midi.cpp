/* Midi functionality - built on top of RtMidi */

#include <iostream>
#include <cstdlib>
#include <string>
#include "rtmidi/RtMidi.h"
#include "midi.h"

class Midi {

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


    // TODO - fill this decoder out
    void midi_msg_cb(double deltatime, std::vector<unsigned char>* message, void* /*userData*/) {
        unsigned int nBytes = message->size();
        // for (unsigned int i = 0; i < nBytes; i++)
        //     std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
        // if (nBytes > 0)
        //     std::cout << "stamp = " << deltatime << std::endl;

        switch((int)message->at(1))
        {
            case 10: // Volume
                currentVolumeKnob = (int)message->at(2);
                break;

            default:
                break;

        }

    }

    std::string getRadioModel(){
        return "RSP1B";
    }

    // Takes a raw knob value (0-127) and scales it between the min and max given
    // returns the scaled value.
    float scaleKnob(int knobValue, float minValue, float maxValue){
        float sf =(maxValue - minValue) / 127.0f;
        return( ((float)knobValue * sf) + minValue );
    }



public:

    // Inits the MIDI controller by name (or partial part of name), Returns true if all went OK.
    bool init(std::string desired_controller_name) {
        try {
            midiin = new RtMidiIn();

            unsigned int nPorts = midiin->getPortCount();

            for (unsigned int i = 0; i < nPorts; i++) {
                std::string portName = midiin->getPortName(i);

                if (portName.find(desired_controller_name) != std::string::npos) {
                    // Found the name - try to open the port number
                    midiin->openPort(i);

                    // Set our callback function.  This should be done immediately after
                    // opening the port to avoid having incoming messages written to the
                    // queue instead of sent to the callback function.
                    midiin->setCallback(&midi_msg_cb);

                    // TODO - Figure out what this really does
                    // Don't ignore sysex, timing, or active sensing messages.
                    midiin->ignoreTypes(false, false, false);

                    // All set, so return
                    midiDisabled = false;
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
    bool getTune(float &value) {
        bool changed = false;

        if(midiDisabled) return false;

        // Finally clear flags and values to reset the control.
        return changed;
    }

    /* Knob: If true, value is the knob position 0-127 */
    bool getVolume(float &scaledValue, float minValue, float maxValue) {
        static int lastVolumeKnob = 0;
        bool changed = false;

        if(midiDisabled) return false;

        if(lastVolumeKnob != currentVolumeKnob){
            scaledValue = scaleKnob(scaledValue, minValue, maxValue);
            lastVolumeKnob = currentVolumeKnob;
            changed = true;
        }
        return changed;
    }

    /* Knob: If true, value is the knob position 0-127 */
    bool getSquelch(float &value, float minValue, float maxValue){
        if(midiDisabled) return false;
        return false;
    }

    /* Knob: If true, value is the knob position 0-127 */
    bool getRfGain(float &value, float minValue, float maxValue) {
        if(midiDisabled) return false;
        return false;
    }

    /* Knob: If true, value is the knob position 0-127 */
    bool getIfGain(float &value, float minValue, float maxValue) {
        if(midiDisabled) return false;
        return false;
    }

    /* Knob: If true, value is the knob position 0-127 */
    bool getPanH(float &value, float minValue, float maxValue) {
        if(midiDisabled) return false;
        return false;
    }

    /* Knob: If true, value is the knob position 0-127 */
    bool getPanL(float &value, float minValue, float maxValue) {
        if(midiDisabled) return false;
        return false;
    }

    /* Button: If true, button was pressed */
    bool getStepPlus() {
        if(midiDisabled) return false;
        return false;
    }

    /* Button: If true, button was pressed */
    bool getStepMinus() {
        if(midiDisabled) return false;
        return false;
    }

};
