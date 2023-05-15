//
//  Biquad.cpp
//
//  Copyright © 2019 Eric Tarr. All rights reserved.
//  Code may not be distributed without permission
//
// Implementation of Audio EQ Cookbook Formulas for 2nd-order filter
// https://www.w3.org/2011/audio/audio-eq-cookbook.html

#include "BiquadHPF.h"



// Define the functions of Biquad
void BiquadHPF::processBuffer(float * samples, const int numSamples, const int channel)
{
    updateCoefficients();
    // Perform the processing
    for (int n = 0; n < numSamples ; n++){
        float x = samples[n];
        
        samples[n] = processSample(x,channel);
    }
}

float BiquadHPF::processSample(float x, int channel)
{
    // Output, processed sample (Direct Form 1)
    float y = (b0 / a0) * x + (b1 / a0) * x1[channel] + (b2 / a0) * x2[channel]
            + (-a1 / a0) * y1[channel] + (-a2 / a0) * y2[channel];

    x2[channel] = x1[channel]; // store delay samples for next process step
    x1[channel] = x;
    y2[channel] = y1[channel];
    y1[channel] = y;
    return y;
};

void BiquadHPF::setFs(float sampleRate)
{
    this->Fs = sampleRate;
    updateCoefficients(); // Need to update if Fs changes
};

float BiquadHPF::getFs()
{
    return Fs;
};

void BiquadHPF::setFreq(float freq)
{
    
    this->freq = freq;
    
    // Use this to adjust for Bilinear Transform:
    //fc = (2.0f*Fs) * std::tan(fc/(Fs*2.0f)); //Frequency Warping

    w0 = (float) (2.0 * M_PI) * freq / Fs; // Normalize frequency

    // Bandwidth/slope/resonance parameter
    alpha = std::sin(w0) / (2.0f * Q);

    updateCoefficients();
};

float BiquadHPF::getFreq()
{
    return freq;
};

void BiquadHPF::setQ(float Q)
{
    this->Q = Q;

    // Bandwidth/slope/resonance parameter
    alpha = std::sin(w0) / (2.0f * Q);

    updateCoefficients();
};

float BiquadHPF::getQ()
{
    return Q;
};

void BiquadHPF::setAmpdB(float ampdB)
{
    this->ampdB = ampdB;

    A = std::pow(10.0f, ampdB / 40.0f); // Linear amplitude

    updateCoefficients();
};

float BiquadHPF::getAmpdB()
{
    return ampdB;
};

void BiquadHPF::updateCoefficients()
{
    switch (filterType)
    {
        case LPF:
        b0 = (1.0f - std::cos(w0)) / 2.0f;
            b1 = 1.0f - std::cos(w0);
            b2 = (1.0f - std::cos(w0)) / 2.0f;
            a0 = 1.0f + alpha;
            a1 = -2.0f * cos(w0);
            a2 = 1.0f - alpha;
            break;

        case HPF:
            b0 = (1.0f + std::cos(w0)) / 2.0f;
            b1 = -(1.0f + std::cos(w0));
            b2 = (1.0f + std::cos(w0)) / 2.0f;
            a0 = 1.0f + alpha;
            a1 = -2.0f * std::cos(w0);
            a2 = 1.0f - alpha;
            break;

        case BPF1:
            b0 = std::sin(w0) / 2.0f;
            b1 = 0.0f;
            b2 = -std::sin(w0) / 2.0f;
            a0 = 1.0f + alpha;
            a1 = -2.0f * std::cos(w0);
            a2 = 1.0f - alpha;
            break;

        case BPF2:
            b0 = alpha;
            b1 = 0.0f;
            b2 = -alpha;
            a0 = 1.0f + alpha;
            a1 = -2.0f * std::cos(w0);
            a2 = 1.0f - alpha;
            break;

        case PEAK:
            b0 = 1.0f + alpha * A;
            b1 = -2.0f * std::cos(w0);
            b2 = 1.0f - alpha * A;
            a0 = 1.0f + alpha / A;
            a1 = -2.0f * std::cos(w0);
            a2 = 1.0f - alpha / A;
            break;

        case NOTCH:
            b0 = 1.0f;
            b1 = -2.0f * std::cos(w0);
            b2 = 1.0f;
            a0 = 1.0f + alpha;
            a1 = -2.0f * std::cos(w0);
            a2 = 1.0f - alpha;
            break;

        case APF:
            b0 = 1.0f - alpha;
            b1 = -2.0f * std::cos(w0);
            b2 = 1.0f + alpha;
            a0 = 1.0f + alpha;
            a1 = -2.0f * std::cos(w0);
            a2 = 1.0f - alpha;
            break;

        case LSHELF:
            b0 = A * ((A + 1.0f) - (A - 1.0f) * std::cos(w0) + 2.0f * std::sqrt(A) * alpha);
            b1 = 2.0f * A * ((A - 1.0f) - (A + 1.0f) * std::cos(w0));
            b2 = A * ((A + 1.0f) - (A - 1.0f) * std::cos(w0) - 2.0f * std::sqrt(A) * alpha);
            a0 = (A + 1.0f) + (A - 1.0f) * std::cos(w0) + 2.0f * std::sqrt(A) * alpha;
            a1 = -2.0f * ((A - 1.0f) + (A + 1.0f) * std::cos(w0));
            a2 = (A + 1.0f) + (A - 1.0f) * std::cos(w0) - 2.0f * std::sqrt(A) * alpha;
            break;

        case HSHELF:
            b0 = A * ((A + 1.0f) + (A - 1.0f) * std::cos(w0) + 2.0f * std::sqrt(A) * alpha);
            b1 = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * std::cos(w0));
            b2 = A * ((A + 1.0f) + (A - 1.0f) * std::cos(w0) - 2.0f * std::sqrt(A) * alpha);
            a0 = (A + 1.0f) - (A - 1.0f) * std::cos(w0) + 2.0f * std::sqrt(A) * alpha;
            a1 = 2.0f * ((A - 1.0f) - (A + 1.0f) * std::cos(w0));
            a2 = (A + 1.0f) - (A - 1.0f) * std::cos(w0) - 2.0f * std::sqrt(A) * alpha;
            break;

        default: // LPF
            b0 = (1.0f - std::cos(w0)) / 2.0f;
            b1 = 1.0f - std::cos(w0);
            b2 = (1.0f - std::cos(w0)) / 2.0f;
            a0 = 1.0f + alpha;
            a1 = -2.0f * std::cos(w0);
            a2 = 1.0f - alpha;
            break;
    }
};
