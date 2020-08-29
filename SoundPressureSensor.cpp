#pragma once

#include "ShootThread.h"
#include <SFML/Audio.hpp>
#include "SystemClock.h"

class SoundPressureSensor : public sf::SoundRecorder
{
private:
    ShootThread* sThread;
    uint64_t last_trigger_time = 0;
    bool ready2Trigger = false;
    double TRIGGER_DB = 0;

public:
    SoundPressureSensor(ShootThread* sThread, double TRIGGER_DB) {
        this->sThread = sThread;
        this->TRIGGER_DB = TRIGGER_DB;
    }

private:
    virtual bool onStart() // optional
    {
        setProcessingInterval(sf::milliseconds(1));
        return true;
    }

    virtual bool onProcessSamples(const sf::Int16* samples, size_t sampleCount)
    {
        uint64_t curr_time = SystemClock::getCurrentTimeMillis();

        float sos = 0.0;
        for (int i = 0; i < sampleCount; i++) {
            float currentSample = (float)*samples++;
            sos += currentSample * currentSample;
        }

        sos = sqrtf(sos / sampleCount);
        float dB = 20 * log10f(sos);

        double interval = SystemClock::getElapsedSeconds(curr_time, last_trigger_time);
        //cout << interval << endl;
        if ((interval > SILENCE_DURATION || last_trigger_time == 0) && !ready2Trigger) {
            ready2Trigger = true;
        }

        //cout << dB << " dB" << endl;

        if (dB > TRIGGER_DB) {
            if (interval > SILENCE_DURATION || last_trigger_time == 0) {
                sThread->audio_trigger();
                ready2Trigger = false;
            }

            last_trigger_time = curr_time;
        }

        return true;
    }
};
