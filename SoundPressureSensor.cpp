#pragma once

#include "RecordThread.h"
#include <portaudio.h>
#include <thread>
#include <math.h>
#include "SystemClock.h"
#include "Utils.h"
#include <QDebug>

using namespace std;

class SoundPressureSensor
{
private:
    RecordThread* rThread;
    uint64_t last_trigger_time = 0;
    bool ready2Trigger = false;
    double TRIGGER_DB = 0;
    thread *micThread;
    bool stopRecording = false;
    int device_index;

public:
    SoundPressureSensor(RecordThread* rThread, double TRIGGER_DB, int device_index) {
        this->rThread = rThread;
        this->TRIGGER_DB = TRIGGER_DB;
        this->device_index = device_index;
    }

    void start() {
        micThread = new thread(&SoundPressureSensor::run, this);
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
                  this);

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

    static int dbCallback( const void *inputBuffer, void *outputBuffer,
                               unsigned long framesPerBuffer,
                               const PaStreamCallbackTimeInfo* timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void *userData ) {

        (void)outputBuffer;
        (void)timeInfo;
        (void)statusFlags;

        SoundPressureSensor *sensor = (SoundPressureSensor *)userData;
        uint64_t curr_time = SystemClock::getCurrentTimeMillis();

        const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
        float sos = 0.0;
        for (unsigned long i = 0; i < framesPerBuffer; i++) {
            float sample = *(float *)rptr;
            sos += sample * sample;
            rptr += 1;
        }

        sos = sqrtf(sos / framesPerBuffer);
        float dB = 20 * log10f(sos / 0.0001);

        double interval = SystemClock::getElapsedMillis(curr_time, sensor->last_trigger_time);
        //cout << interval << endl;
        if ((interval > SILENCE_DURATION ||
            sensor->last_trigger_time == 0) &&
            !sensor->ready2Trigger) {
            sensor->ready2Trigger = true;
        }

        //cout << dB << " dB" << endl;

        if (dB > sensor->TRIGGER_DB) {
            if (interval > SILENCE_DURATION ||
                sensor->last_trigger_time == 0) {
                sensor->rThread->audio_trigger(curr_time);
                sensor->ready2Trigger = false;
            }

            sensor->last_trigger_time = curr_time;
        }

        return 0;
    }
};
