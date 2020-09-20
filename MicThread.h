#ifndef MICTHREAD_H
#define MICTHREAD_H

#include <SFML/Audio.hpp>
#include "Utils.h"
#include <math.h>

class MicThread : public sf::SoundRecorder {
private:
    std::function<void(float)> updateSamples;

public:
    MicThread(std::function<void(float)> updateSamples) {
        this->updateSamples = updateSamples;
    }

private:
    virtual bool onStart() // optional
    {
        setProcessingInterval(sf::milliseconds(33));
        return true;
    }

    virtual bool onProcessSamples(const sf::Int16* samples, size_t sampleCount)
    {
        float sos = 0.0;
        for (size_t i = 0; i < sampleCount; i++) {
            float currentSample = (float)*samples++;
            sos += currentSample * currentSample;
        }

        sos = sqrtf(sos / sampleCount);
        float dB = 20 * log10f(sos);

        updateSamples(dB);
        return true;
    }
};

#endif // MICTHREAD_H
