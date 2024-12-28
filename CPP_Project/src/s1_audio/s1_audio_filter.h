/*

single file, multipurpose filter

only usable from c++

*/
#ifndef S1_AUDIO_FILTER_H
#define S1_AUDIO_FILTER_H

#include <macros.h>  // my macros to help declare properties
#include <helper.h>  // includes a print function

// #include <godot_cpp/classes/audio_stream.hpp>  // AudioStreamPlayer

using namespace godot;

// // RC-like envelope follower
// class AnalogPeakSimulator {
//    private:
//     float peak;       // Current peak value
//     float decayRate;  // How quickly the peak falls

//    public:
//     AnalogPeakSimulator(float initialPeak = 0.0f, float decay = 0.01f)
//         : peak(initialPeak), decayRate(decay) {}

//     float process(float input) {
//         // Simulate voltage stability (smooth peak-following)
//         if (input > peak) {
//             peak = input;  // Instant peak rise
//         } else {
//             peak -= decayRate * (peak - input);  // Smooth decay
//         }
//         return peak;
//     }
// };

class AnalogPeakSimulator {
   public:
    float peak;         // Current peak value
    float decay_rate;   // How quickly the peak falls
    float sample_rate;  // Sample rate to adjust decay timing

    // Constructor with initial peak, decay rate, and sample rate
    AnalogPeakSimulator(float initialPeak = 0.0f, float decay = 0.01f, float sampleRate = 44100.0f)
        : peak(initialPeak), decay_rate(decay), sample_rate(sampleRate) {}

    // Process the input sample
    float process(float input) {
        // Adjust decay rate based on sample rate
        float adjustedDecayRate = decay_rate / sample_rate;

        // Simulate voltage stability (smooth peak-following)
        if (input > peak) {
            peak = input;  // Instant peak rise
        } else {
            peak -= adjustedDecayRate * (peak - input);  // Smooth decay
        }
        return peak;
    }
};

// new from chat gp:
// Improved Low-Pass Filter with Resonance
//
class LowPassFilter {
   public:
    LowPassFilter(float sampleRate, float cutoffFrequency, float resonance)
        : _sampleRate(sampleRate), _cutoffFrequency(cutoffFrequency), _resonance(resonance) {
        calculateCoefficients();
    }

    void set_cutoff(float cutoff) {
        _cutoffFrequency = cutoff;
        calculateCoefficients();
    }

    void set_sample_rate(float sampleRate) {
        _sampleRate = sampleRate;
        calculateCoefficients();
    }

    void set_resonance(float resonance) {
        _resonance = resonance;
    }

    float process(float input) {
        // State variable filter equations
        float bandPass = _band + _cutoff * (input - _low - _resonance * _band);
        float lowPass = _low + _cutoff * _band;

        _band = bandPass;
        _low = lowPass;

        return _low;
    }

   private:
    float _sampleRate;
    float _cutoffFrequency;
    float _resonance;
    float _cutoff;
    float _low = 0.0f;
    float _band = 0.0f;

    void calculateCoefficients() {
        _cutoff = 2.0f * sinf(3.14159f * _cutoffFrequency / _sampleRate);
    }
};

// multi purpose filter
class S1AudioFilter {
   private:
    float _frequency;           // Cutoff frequency in Hz
    float _sample_rate;         // Sample rate in Hz
    float _alpha;               // Filter coefficient
    float _resonance = 0.0f;    // Resonance amount
    float _q = 1.0f;            // Quality factor (bandwidth control)
    float _prev_input = 0.0f;   // Previous input sample
    float _prev_output = 0.0f;  // Previous output sample
    float _low_output = 0.0f;   // Low-pass state variable
    float _high_output = 0.0f;  // High-pass state variable
    float _band_output = 0.0f;  // Band-pass state variable

#pragma region TEST_PROPS
   private:
    int _value;

   public:
    // Getter
    operator int() const {
        return _value;
    }

    // Setter
    S1AudioFilter& operator=(int value) {
        _value = value;
        return *this;
    }

#pragma endregion

   public:
    enum Mode { LOW,
                HIGH,
                BAND,
                NOTCH };  // Add notch as a new mode

    Mode _mode = Mode::HIGH;  // Default to high-pass

    void _update_alpha() {
        float omega = Math_TAU * _frequency / _sample_rate;
        _alpha = sin(omega) / (2.0f * _q);
    }

   public:
    S1AudioFilter(float frequency, float sample_rate, Mode mode = Mode::HIGH, float q = 1.0f)
        : _frequency(frequency), _sample_rate(sample_rate), _mode(mode), _q(q) {
        _update_alpha();
    }

    void set_cutoff(float cutoff) {
        _frequency = cutoff;
        _update_alpha();
    }

    void set_sample_rate(float sample_rate) {
        _sample_rate = sample_rate;
        _update_alpha();
    }

    void set_resonance(float resonance) {
        _resonance = resonance;
    }

    void set_quality(float q) {
        _q = q;
        _update_alpha();
    }

    void set_mode(Mode mode) {
        _mode = mode;
    }

    float process(float input) {
        float output = 0.0f;

        switch (_mode) {
            case Mode::LOW:
                // Low-pass filter
                _low_output = _prev_output + _alpha * (input - _prev_output) - _resonance * _prev_output;
                output = _low_output;
                break;
            case Mode::HIGH:
                // High-pass filter
                // _high_output = _alpha * (_prev_output + input - _prev_input) - _resonance * _prev_output;
                // output = _high_output;

                _low_output = _prev_output + _alpha * (input - _prev_output);  // Update low-pass
                _high_output = input - _low_output;                            // Subtract low-pass from input
                output = _high_output - _resonance * _high_output;             // Add resonance effect

                break;
            case Mode::BAND:
                // Band-pass filter
                _band_output += _alpha * (input - _band_output);
                output = _band_output - _resonance * _band_output / _q;
                break;
            case Mode::NOTCH:
                // Notch filter: Combine low-pass and high-pass to "notch out" frequencies

                // note does not use the Q

                _low_output = _prev_output + _alpha * (input - _prev_output) - _resonance * _prev_output;
                _high_output = _alpha * (_prev_output + input - _prev_input) - _resonance * _prev_output;
                output = _low_output + _high_output;  // Combine low-pass and high-pass

                // CHAT GP DOESN'T SEEM TO UNDERSTAND HOW TO ADD Q

                // // Notch filter: Combines band-pass behavior to reject specific frequency range
                // float band = _band_output + _alpha * (input - _band_output);   // Band-pass part
                // _low_output = _prev_output + _alpha * (input - _prev_output);  // Low-pass
                // _high_output = _alpha * (_prev_output + input - _prev_input);  // High-pass

                // output = input - band / _q;  // Subtract the band component

                break;
        }

        // Update state
        _prev_input = input;
        _prev_output = output;
        return output;
    }
};

#endif