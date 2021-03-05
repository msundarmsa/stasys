#ifndef MICTHREAD_H
#define MICTHREAD_H

#include "Utils.h"
#include <math.h>
#include <portaudio.h>
#include <thread>
#include <QDebug>

using namespace std;

class MicThread {
private:
    function<void(float)> updateSamples;
    thread *micThread;
    int device_index;
    bool stopRecording = false;

public:
    MicThread(function<void(float)> updateSamples, int device_index) {
        this->updateSamples = updateSamples;
        this->device_index = device_index;
    }

    void start() {
        micThread = new thread(&MicThread::run, this);
    }

    void stop() {
        if (!stopRecording) {
            stopRecording = true;
            if (micThread->joinable()) {
                micThread->join();
            }
        }
    }

private:
    static int dbCallback( const void *inputBuffer, void *outputBuffer,
                               unsigned long framesPerBuffer,
                               const PaStreamCallbackTimeInfo* timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void *userData ) {

        (void)outputBuffer;
        (void)timeInfo;
        (void)statusFlags;

        function<void(float)> updateSamples = *(function<void(float)> *)userData;

        const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
        float sos = 0.0;
        for (unsigned long i = 0; i < framesPerBuffer; i++) {
            float sample = *(float *)rptr;
            sos += sample * sample;
            rptr += 1;
        }

        sos = sqrtf(sos / framesPerBuffer);
        float dB = 20 * log10f(sos / 0.0001);
        updateSamples(dB);

        return 0;
    }

    void run() {
        PaStreamParameters inputParameters;
        inputParameters.device = device_index;
        inputParameters.channelCount = 1;
        inputParameters.sampleFormat = PA_SAMPLE_TYPE;
        inputParameters.suggestedLatency = Pa_GetDeviceInfo(device_index)->defaultLowInputLatency;
        inputParameters.hostApiSpecificStreamInfo = NULL;

        PaStream* stream;
        int err = Pa_OpenStream(
                  &stream,
                  &inputParameters,
                  NULL,                  /* &outputParameters, */
                  SAMPLE_RATE,
                  FRAMES_PER_BUFFER * 5,
                  paClipOff,      /* we won't output out of range samples so don't bother clipping them */
                  dbCallback,
                  &(this->updateSamples));

        if (err != paNoError) {
            qDebug() << "ERROR: Could not open input stream: (" << err << " )";
            return;
        }

        err = Pa_StartStream( stream );
        if (err != paNoError) {
            qDebug() << "ERROR: Could not start input stream: (" << err << " )";
            return;
        }

        while( ( err = Pa_IsStreamActive( stream ) ) == 1 && !stopRecording)
        {
            Pa_Sleep(100);
        }

        err = Pa_CloseStream( stream );
        if (err != paNoError) {
            qDebug() << "ERROR: Could not close input stream: (" << err << ")";
            return;
        }
    }
};

#endif // MICTHREAD_H
