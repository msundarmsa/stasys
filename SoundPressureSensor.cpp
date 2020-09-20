#pragma once

#include "RecordThread.h"
#include <SFML/Audio.hpp>
#include "SystemClock.h"
#include "Utils.h"

using namespace sf;

class SoundPressureSensor : public SoundRecorder
{
private:
    RecordThread* rThread;
    uint64_t last_trigger_time = 0;
    bool ready2Trigger = false;
    double TRIGGER_DB = 0;

public:
    SoundPressureSensor(RecordThread* rThread, double TRIGGER_DB) {
        this->rThread = rThread;
        this->TRIGGER_DB = TRIGGER_DB;
    }

private:
    virtual bool onStart() // optional
    {
        setProcessingInterval(milliseconds(1));
        return true;
    }

    virtual bool onProcessSamples(const Int16* samples, size_t sampleCount)
    {
        uint64_t curr_time = SystemClock::getCurrentTimeMillis();

        float sos = 0.0;
        for (size_t i = 0; i < sampleCount; i++) {
            float currentSample = (float)*samples++;
            sos += currentSample * currentSample;
        }

        sos = sqrtf(sos / sampleCount);
        float dB = 20 * log10f(sos);

        double interval = SystemClock::getElapsedMillis(curr_time, last_trigger_time);
        //cout << interval << endl;
        if ((interval > SILENCE_DURATION || last_trigger_time == 0) && !ready2Trigger) {
            ready2Trigger = true;
        }

        //cout << dB << " dB" << endl;

        if (dB > TRIGGER_DB) {
            if (interval > SILENCE_DURATION || last_trigger_time == 0) {
                rThread->audio_trigger(curr_time);
                ready2Trigger = false;
            }

            last_trigger_time = curr_time;
        }

        return true;
    }
};
